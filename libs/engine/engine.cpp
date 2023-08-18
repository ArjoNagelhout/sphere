#include "engine.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

namespace engine {

    Engine *engine;

    void FrameData::initialize() {
        // create synchronization primitives
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        checkResult(vkCreateSemaphore(engine->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
        checkResult(vkCreateSemaphore(engine->device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));
        checkResult(vkCreateFence(engine->device, &fenceCreateInfo, nullptr, &inFlightFence));

        std::cout << "created frame data" << std::endl;
    }

    void FrameData::destroy() const {
        vkDestroySemaphore(engine->device, imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(engine->device, renderFinishedSemaphore, nullptr);
        vkDestroyFence(engine->device, inFlightFence, nullptr);

        std::cout << "destroyed frame data" << std::endl;
    }

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        engine->framebufferResized = true;
        engine->render();
        // std::cout << "frame buffer resized to x: " << width << ", y: " << height << std::endl;
    }

    Engine::Engine(EngineConfiguration &engineConfiguration) {

        assert((engine == nullptr) && "Only one engine can exist at one time");
        engine = this;

        std::cout << "created engine" << std::endl;

        configuration = {
                .window = engineConfiguration.window,

                .engineName = ENGINE_NAME,
                .applicationName = engineConfiguration.applicationName,
                .engineVersion = ENGINE_VERSION,
                .applicationVersion = engineConfiguration.applicationVersion,

                .debug = engineConfiguration.debug,

                .preferredSurfaceFormats = {},
                .requiredInstanceExtensions = {},
                .requiredInstanceLayers = {},
                .requiredDeviceExtensions = {},
        };

        std::vector<const char *> allRequiredInstanceExtensions{configuration.requiredInstanceExtensions.begin(),
                                                                configuration.requiredInstanceExtensions.end()};
        std::vector<const char *> allRequiredInstanceLayers{configuration.requiredInstanceLayers.begin(),
                                                            configuration.requiredInstanceLayers.end()};

        if (configuration.debug) {
            allRequiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            allRequiredInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        createInstance(allRequiredInstanceExtensions, allRequiredInstanceLayers);
        createDebugMessenger();
        createSurface();
        pickPhysicalDevice(requiredDeviceExtensions);
        createDevice(requiredDeviceExtensions);

        glfwSetFramebufferSizeCallback(configuration.window, framebufferResizeCallback);

        allocator = std::make_unique<MemoryAllocator>();
        swapchain = std::make_unique<Swapchain>(preferredSurfaceFormats);
        renderPass = std::make_unique<RenderPass>(swapchain->surfaceFormat.format, depthImageFormat);
        descriptorSetBuilder = std::make_unique<DescriptorSetBuilder>();
        pipelineBuilder = std::make_unique<PipelineBuilder>();

        createDepthImage();
        swapchain->createFramebuffers(renderPass->renderPass, depthImageView);
        camera = std::make_unique<Camera>(*allocator, *swapchain);

        createCommandPool();
        std::vector<VkCommandBuffer> commandBuffers = createCommandBuffers();
        uploadCommandBuffer = commandBuffers[MAX_FRAMES_IN_FLIGHT]; // use the command buffer that comes after the frame command buffers

        VkFenceCreateInfo fenceInfo{
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0
        };
        vkCreateFence(device, &fenceInfo, nullptr, &uploadFence);

        // load image
        texture = std::make_unique<Texture>(
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-11_green_assets/textures/edited/leaves_1.png");

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            std::vector<VkDescriptorSet> descriptorSets = descriptorSetBuilder->createDescriptorSets(
                    descriptorSetBuilder->descriptorSetLayout, 1);
            descriptorSetBuilder->bindBuffer(descriptorSets[0], camera->cameraDataBuffer, 0);
            descriptorSetBuilder->bindImage(descriptorSets[0], texture->sampler, texture->imageView, 1);

            FrameData frameData{
                    .commandBuffer = commandBuffers[i],
                    .descriptorSets = descriptorSets
            };
            frameData.initialize();
            frames.push_back(frameData);
        }

        // load meshes
        std::vector<std::string> meshNames{
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/orb.obj",
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/hollowcube.obj",
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/bunny_patched.obj",
                "/Users/arjonagelhout/Downloads/kenney_platformer-kit/Models/OBJ format/blockSnowRoundedLow.obj",
                "/Users/arjonagelhout/Downloads/kenney_platformer-kit/Models/OBJ format/ladderBroken.obj",
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/bunny.obj",
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/column.obj",
                "/Users/arjonagelhout/Documents/ShapeReality/sphere/external/tinyobjloader/models/map-bump.obj",
        };

        for (const auto &meshName: meshNames) {
            meshes.emplace_back(std::make_unique<Mesh>(meshName));
        }

        // load shaders
        struct ShaderData {
            std::string vertexShaderPath;
            std::string fragmentShaderPath;
        };

        std::vector<ShaderData> shadersData{
                {"shader_vert.spv", "shader_frag.spv"},
                {"shader_test_vert.spv", "shader_test_frag.spv"}
        };

        for (const auto &shaderData: shadersData) {
            shaders.emplace_back(std::make_unique<Shader>(shaderData.vertexShaderPath, shaderData.fragmentShaderPath));
        }

        // create scene with objects
        struct ObjectData {
            glm::vec3 position;
            glm::vec3 scale;
            Mesh &mesh;
            Shader &shader;
        };

        std::vector<ObjectData> objectsData{
                {{0, 0,  0}, {1,   1,    1},    *meshes[0], *shaders[0]},
                {{0, 2,  0}, {0.9, 1,    1},    *meshes[0], *shaders[1]},
                {{0, 4,  0}, {0.8, 1,    1},    *meshes[0], *shaders[1]},
                {{0, 6,  0}, {0.7, 1,    1},    *meshes[0], *shaders[1]},
                {{0, 8,  0}, {0.6, 1,    1},    *meshes[0], *shaders[0]},
                {{0, 10, 0}, {0.5, 1,    1},    *meshes[0], *shaders[0]},
                {{0, 12, 0}, {0.4, 1,    1},    *meshes[0], *shaders[0]},
                {{0, 14, 0}, {0.3, 1,    1},    *meshes[0], *shaders[0]},
                {{4, 0,  0}, {1,   0.25, 0.25}, *meshes[1], *shaders[1]},
                {{4, 2,  0}, {0.9, 0.25, 0.25}, *meshes[1], *shaders[1]},
                {{4, 4,  0}, {0.8, 0.25, 0.25}, *meshes[1], *shaders[1]},
                {{4, 6,  0}, {0.7, 0.25, 0.25}, *meshes[1], *shaders[0]},
                {{4, 8,  0}, {0.6, 0.25, 0.25}, *meshes[1], *shaders[0]},
                {{4, 10, 0}, {0.5, 0.25, 0.25}, *meshes[1], *shaders[0]},
                {{4, 12, 0}, {0.4, 0.25, 0.25}, *meshes[1], *shaders[0]},
                {{4, 14, 0}, {0.3, 0.25, 0.25}, *meshes[1], *shaders[0]},
        };

        for (const auto &objectData: objectsData) {
            objects.emplace_back(std::make_unique<Object>(objectData.mesh, objectData.shader));
            const auto &obj = objects.back();
            obj->localPosition = objectData.position;
            obj->localScale = objectData.scale;
        }

        initializeImgui();
    }

    Engine::~Engine() {
        vkDeviceWaitIdle(device);

        destroyImgui();

        for (auto const &frameData: frames) {
            frameData.destroy();
        }

        vkDestroyFence(device, uploadFence, nullptr);
        vkDestroyCommandPool(device, commandPool, nullptr);

        // destroy depth image
        vkDestroyImageView(device, depthImageView, nullptr);
        vmaDestroyImage(allocator->allocator, depthImage, depthImageAllocation);

        texture.reset();
        meshes.clear();

        pipelineBuilder.reset();
        descriptorSetBuilder.reset();
        renderPass.reset();
        swapchain.reset();
        allocator.reset();

        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroyDebugMessenger();
        vkDestroyInstance(instance, nullptr);

        std::cout << "destroyed engine" << std::endl;
    }

    void Engine::render() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        renderImgui();
        ImGui::Render();

        camera->updateCameraData();

        // update mesh transforms
        for (size_t i = 0; i < objects.size(); i++) {
            const auto &object = objects[i];
            object->localRotation = object->localRotation *
                                    glm::angleAxis(0.05f * (-1.0f + 2.0f * static_cast<float>(i % 2)),
                                                   glm::vec3(0, 1, 0));
        }
        drawFrame();
    }

    void Engine::drawFrame() {
        const FrameData &frameData = frames[currentFrameIndex];

        VkResult result;

        vkWaitForFences(device, 1, &frameData.inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        result = vkAcquireNextImageKHR(device,
                                       swapchain->swapchain,
                                       UINT64_MAX,
                                       frameData.imageAvailableSemaphore,
                                       VK_NULL_HANDLE,
                                       &imageIndex);
        switch (result) {
            case VK_SUCCESS:
            case VK_ERROR_OUT_OF_DATE_KHR:
            case VK_SUBOPTIMAL_KHR:
                break;
//            case VK_ERROR_OUT_OF_DATE_KHR:
//                window resized, so we should recreate the swapchain
//                swapchain->recreate();
//                return;
            default:
                throw std::runtime_error(
                        std::string("failed to acquire swapchain image") + string_VkResult(result));
        }

        vkResetFences(device, 1, &frameData.inFlightFence);

        // frame buffer must have been created with the same render pass (compatibility)
        VkFramebuffer &framebuffer = swapchain->framebuffers[imageIndex];

        vkResetCommandBuffer(frameData.commandBuffer, 0);
        recordCommandBuffer(frameData, framebuffer);

        VkSemaphore waitSemaphores[] = {frameData.imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signalSemaphores[] = {frameData.renderFinishedSemaphore};

        VkSubmitInfo submitInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = waitSemaphores,
                .pWaitDstStageMask = waitStages,
                .commandBufferCount = 1,
                .pCommandBuffers = &frameData.commandBuffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = signalSemaphores,
        };
        checkResult(vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameData.inFlightFence));

        VkSwapchainKHR swapchains[] = {swapchain->swapchain};

        VkPresentInfoKHR presentInfo{
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = signalSemaphores,
                .swapchainCount = 1,
                .pSwapchains = swapchains,
                .pImageIndices = &imageIndex,
                .pResults = nullptr,
        };
        result = vkQueuePresentKHR(presentQueue, &presentInfo);
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
            swapchain->recreate();
            framebufferResized = false;
        } else {
            checkResult(result);
        }

        currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Engine::recordCommandBuffer(const FrameData &frameData, const VkFramebuffer &framebuffer) {
        const VkCommandBuffer &cmd = frameData.commandBuffer;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        checkResult(vkBeginCommandBuffer(cmd, &beginInfo));

        VkClearValue clearColor = {.color = {{0.757f, 0.953f, 1.0f, 1.0f}}};
        VkClearValue clearDepth = {.depthStencil{.depth = 1.0f}};
        VkClearValue clearValues[] = {
                clearColor,
                clearDepth
        };
        VkExtent2D extent = swapchain->extent;

        VkRenderPassBeginInfo renderPassInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = renderPass->renderPass,
                .framebuffer = framebuffer,
                .renderArea = {
                        .offset = {0, 0},
                        .extent = extent
                },
                .clearValueCount = 2,
                .pClearValues = clearValues,
        };

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(extent.width),
                .height = static_cast<float>(extent.height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f};
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{
                .offset = {0, 0},
                .extent = extent};
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        for (const auto &object: objects) {

            // bind the pipeline
            PipelineData *pipelineData = object->shader.pipelineData;

            vkCmdBindDescriptorSets(cmd,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineData->pipelineLayout,
                                    0,
                                    static_cast<uint32_t>(frameData.descriptorSets.size()),
                                    frameData.descriptorSets.data(),
                                    0,
                                    nullptr);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->pipeline);

            // push transform matrix using push constants
            glm::mat4x4 transform = object->getTransform();
            vkCmdPushConstants(cmd,
                               pipelineData->pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT,
                               0, sizeof(transform), &transform);
            VkDeviceSize vertexBufferOffset = 0;
            vkCmdBindIndexBuffer(cmd, object->mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindVertexBuffers(cmd, 0, 1, &(object->mesh.vertexBuffer), &vertexBufferOffset);
            vkCmdDrawIndexed(cmd, static_cast<uint32_t>(object->mesh.indices.size()), 1, 0, 0, 0);
        }

        // imgui
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

        vkCmdEndRenderPass(cmd);
        checkResult(vkEndCommandBuffer(cmd));
    }

    void Engine::createCommandPool() {
        QueueFamiliesData data = queueFamiliesData;

        VkCommandPoolCreateInfo info{
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = data.graphicsQueueFamilyData->index,
        };
        checkResult(vkCreateCommandPool(device, &info, nullptr, &commandPool));
        std::cout << "created command pool" << std::endl;
    }

    std::vector<VkCommandBuffer> Engine::createCommandBuffers() {
        std::vector<VkCommandBuffer> commandBuffers(MAX_FRAMES_IN_FLIGHT + UPLOAD_COMMAND_BUFFERS);

        VkCommandBufferAllocateInfo allocateInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = commandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
        };
        checkResult(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()));
        std::cout << "created command buffers" << std::endl;

        return commandBuffers;
    }

    void Engine::createDepthImage() {
        VkExtent3D extent = toExtent3D(swapchain->extent);
        VkImageCreateInfo imageInfo = vk_create::image(depthImageFormat, extent,
                                                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        VmaAllocationCreateInfo allocationInfo{
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                .usage = VMA_MEMORY_USAGE_GPU_ONLY,
                .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };
        checkResult(vmaCreateImage(allocator->allocator,
                                   &imageInfo, &allocationInfo,
                                   &depthImage, &depthImageAllocation, nullptr));

        VkImageViewCreateInfo imageViewInfo = vk_create::imageView(depthImage, depthImageFormat,
                                                                   VK_IMAGE_ASPECT_DEPTH_BIT);
        checkResult(vkCreateImageView(device, &imageViewInfo, nullptr, &depthImageView));

        std::cout << "created depth image" << std::endl;
    }

    void Engine::immediateSubmit(std::function<void(VkCommandBuffer)> &&function) {
        const VkCommandBuffer &cmd = uploadCommandBuffer;
        const VkFence &fence = uploadFence;

        vkDeviceWaitIdle(device);

        // upload the image to the read only shader layout
        checkResult(vkResetCommandPool(device, commandPool, 0));
        VkCommandBufferBeginInfo beginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        checkResult(vkBeginCommandBuffer(engine->uploadCommandBuffer, &beginInfo));

        function(cmd);

        VkSubmitInfo submitInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = nullptr,
                .commandBufferCount = 1,
                .pCommandBuffers = &cmd,
        };
        checkResult(vkEndCommandBuffer(cmd));
        checkResult(vkQueueSubmit(engine->graphicsQueue, 1, &submitInfo, fence));

        vkWaitForFences(engine->device, 1, &fence, true, UINT64_MAX);
        vkResetFences(engine->device, 1, &fence);
    }

    void Engine::initializeImgui() {
        //1: create descriptor pool for IMGUI
        // the size of the pool is very oversize, but it's copied from imgui demo itself.
        VkDescriptorPoolSize poolSizes[] = {
                {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
        };

        VkDescriptorPoolCreateInfo poolInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                .maxSets = 1000,
                .poolSizeCount = std::size(poolSizes),
                .pPoolSizes = poolSizes,
        };

        checkResult(vkCreateDescriptorPool(device, &poolInfo, nullptr, &imguiDescriptorPool));

        // 2: initialize imgui library

        //this initializes the core structures of imgui
        ImGui::CreateContext();

        // init Imgui
        uint32_t imageCount = static_cast<uint32_t>(swapchain->framebuffers.size());
        ImGui_ImplGlfw_InitForVulkan(configuration.window, true);
        ImGui_ImplVulkan_InitInfo initInfo{
                .Instance = instance,
                .PhysicalDevice = physicalDevice,
                .Device = device,
                .Queue = graphicsQueue,
                .DescriptorPool = imguiDescriptorPool,
                .MinImageCount = imageCount,
                .ImageCount = imageCount,
                .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        };

        ImGui_ImplVulkan_Init(&initInfo, renderPass->renderPass);
        immediateSubmit([&](VkCommandBuffer cmd) {
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        });
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        std::cout << "initialized imgui" << std::endl;
    }

    void Engine::destroyImgui() {
        vkDestroyDescriptorPool(device, imguiDescriptorPool, nullptr);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}