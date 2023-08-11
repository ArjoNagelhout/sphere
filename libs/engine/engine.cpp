#include "engine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

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

    void FrameData::updateDescriptorSet(VkBuffer &buffer, VkDescriptorType descriptorType) const {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = descriptorType;
        writeDescriptorSet.pImageInfo = nullptr;
        writeDescriptorSet.pBufferInfo = &bufferInfo;
        writeDescriptorSet.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(engine->device, 1, &writeDescriptorSet, 0, nullptr);
    }

    /*
     * Todo: refactor
     */
    static void loadObj(const std::string &filePath, std::vector<VertexAttributes> &vertices, std::vector<uint32_t> &indices) {

        vertices.clear();
        indices.clear();

        tinyobj::ObjReaderConfig config;
        //config.mtl_search_path = "./";

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filePath, config)) {
            if (!reader.Error().empty()) {
                throw std::runtime_error(
                        std::string("tiny obj reader failed to read from file: ") + reader.Error());
            }
        }

        if (!reader.Warning().empty()) {
            std::cout << "tiny obj reader warning: " << reader.Warning() << std::endl;
        }

        auto &attributes = reader.GetAttrib();
        auto &shapes = reader.GetShapes();
        //auto &materials = reader.GetMaterials();

        for (size_t v = 0; v < attributes.vertices.size(); v += 3) {

            VertexAttributes vertexData{{
                                          attributes.vertices[v + 0],
                                          attributes.vertices[v + 1],
                                          attributes.vertices[v + 2]
                                  }};
            vertices.push_back(vertexData);
        }

        for (size_t s = 0; s < shapes.size(); s++) {
            const std::vector<tinyobj::index_t> &sourceIndices = shapes[s].mesh.indices;

            for (size_t i = 0; i < sourceIndices.size(); i++) {
                const tinyobj::index_t &index = sourceIndices[i];
                indices.push_back(index.vertex_index);
            }
        }

        std::cout << "loaded 3d model at: " << filePath << std::endl;

    //            for (size_t s = 0; s < shapes.size(); s++) {
    //
    //                size_t index_offset = 0;
    //                for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
    //                    size_t vertices = size_t(shapes[s].mesh.num_face_vertices[f]);
    //
    //                    for (size_t v = 0; v < vertices; v++) {
    //                        tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];
    //                        size_t a = 3*size_t(index.vertex_index);
    //                        tinyobj::real_t x = attributes.vertices[a+0];
    //                        tinyobj::real_t y = attributes.vertices[a+1];
    //                        tinyobj::real_t z = attributes.vertices[a+2];
    //                    }
    //                }
    //            }
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

        std::vector<const char *> allRequiredInstanceExtensions{configuration.requiredInstanceExtensions.begin(), configuration.requiredInstanceExtensions.end()};
        std::vector<const char *> allRequiredInstanceLayers{configuration.requiredInstanceLayers.begin(), configuration.requiredInstanceLayers.end()};

        if (configuration.debug) {
            allRequiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            allRequiredInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        createInstance(allRequiredInstanceExtensions, allRequiredInstanceLayers);
        createDebugMessenger();
        createSurface();
        pickPhysicalDevice(requiredDeviceExtensions);
        createDevice(requiredDeviceExtensions);

        swapchain = std::make_unique<Swapchain>(preferredSurfaceFormats);
        renderPass = std::make_unique<RenderPass>(swapchain->surfaceFormat.format);
        swapchain->createFramebuffers(renderPass->renderPass);
        graphicsPipeline = std::make_unique<GraphicsPipeline>(*swapchain, *renderPass);
        allocator = std::make_unique<MemoryAllocator>();
        camera = std::make_unique<Camera>(*allocator, *swapchain);

        createCommandPool();
        createDescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        std::vector<VkCommandBuffer> commandBuffers = allocateCommandBuffers();
        std::vector<VkDescriptorSet> descriptorSets = allocateDescriptorSets(graphicsPipeline->descriptorSetLayout);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            FrameData frameData{
                .commandBuffer = commandBuffers[i],
                .descriptorSet = descriptorSets[i]
            };
            frameData.initialize();
            frames.push_back(frameData);
            frameData.updateDescriptorSet(camera->cameraDataBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        }

        std::string path = "/Users/arjonagelhout/Documents/ShapeReality/sphere/external/tinyobjloader/models/map-bump.obj";
        loadObj(path,
                vertices,
                indices);

        allocator->createBuffer<VertexAttributes>(vertexBuffer,
                                                  vertexBufferAllocation,
                                                  *vertices.data(),
                                                         vertices.size() * sizeof(vertices[0]),
                                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        allocator->createBuffer<uint32_t>(indexBuffer,
                                          indexBufferAllocation,
                                          *indices.data(),
                                                indices.size() * sizeof(indices[0]),
                                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    Engine::~Engine() {
        vkDeviceWaitIdle(device);

        for (auto const &frameData : frames) {
            frameData.destroy();
        }

        vkDestroyCommandPool(device, commandPool, nullptr);
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);

        graphicsPipeline.reset();
        renderPass.reset();
        swapchain.reset();

        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroyDebugMessenger();
        vkDestroyInstance(instance, nullptr);

        std::cout << "destroyed engine" << std::endl;
    }

    void Engine::render() {
        camera->updateCameraData();
        drawFrame(frames[currentFrameIndex]);
        currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Engine::drawFrame(FrameData frameData) {
        VkResult result;

        vkWaitForFences(device, 1, &frameData.inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &frameData.inFlightFence);

        uint32_t imageIndex;
        result = vkAcquireNextImageKHR(device,
                                       swapchain->swapchain,
                                       UINT64_MAX,
                                       frameData.imageAvailableSemaphore,
                                       VK_NULL_HANDLE,
                                       &imageIndex);
        switch (result) {
            case VK_SUCCESS:
            case VK_SUBOPTIMAL_KHR:
                break;
            default:
                throw std::runtime_error(
                        std::string("failed to acquire swapchain image") + string_VkResult(result));
        }

        // frame buffer must have been created with the same render pass (compatibility)
        VkFramebuffer &framebuffer = swapchain->framebuffers[imageIndex];

        vkResetCommandBuffer(frameData.commandBuffer, 0);
        recordCommandBuffer(frameData, framebuffer);

        VkSemaphore waitSemaphores[] = {frameData.imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signalSemaphores[] = {frameData.renderFinishedSemaphore};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frameData.commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        checkResult(vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameData.inFlightFence));

        VkSwapchainKHR swapchains[] = {swapchain->swapchain};

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);
        switch (result) {
            case VK_SUCCESS:
            case VK_SUBOPTIMAL_KHR:
                break;
            default:
                throw std::runtime_error(
                        std::string("failed to present swap chain image") + string_VkResult(result));
        }
    }

    void Engine::recordCommandBuffer(FrameData frameData, VkFramebuffer framebuffer) {
        VkCommandBuffer &cmd = frameData.commandBuffer;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        checkResult(vkBeginCommandBuffer(cmd, &beginInfo));

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkExtent2D extent = swapchain->extent;

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass->renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipeline->graphicsPipelineLayout,
                                0,
                                1,
                                &frameData.descriptorSet,
                                0,
                                nullptr);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);

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

        VkDeviceSize vertexBufferOffset = 0;
        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &vertexBufferOffset);
        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
        vkCmdEndRenderPass(cmd);
        checkResult(vkEndCommandBuffer(cmd));
    }

    void Engine::createCommandPool() {
        QueueFamiliesData data = queueFamiliesData;

        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = data.graphicsQueueFamilyData->index;

        checkResult(vkCreateCommandPool(device, &createInfo, nullptr, &commandPool));
        std::cout << "created command pool" << std::endl;
    }

    void Engine::createDescriptorPool(const VkDescriptorType &descriptorType) {

        VkDescriptorPoolSize poolSize{
                .type = descriptorType,
                .descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        };

        VkDescriptorPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        createInfo.poolSizeCount = 1;
        createInfo.pPoolSizes = &poolSize;

        checkResult(vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool));
        std::cout << "created descriptor pool" << std::endl;
    }

    std::vector<VkCommandBuffer> Engine::allocateCommandBuffers() {
        std::vector<VkCommandBuffer> commandBuffers(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        checkResult(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()));
        std::cout << "created command buffers" << std::endl;

        return commandBuffers;
    }

    std::vector<VkDescriptorSet> Engine::allocateDescriptorSets(VkDescriptorSetLayout &descriptorSetLayout) {
        std::vector<VkDescriptorSet> descriptorSets(MAX_FRAMES_IN_FLIGHT);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSets.size());
        allocateInfo.pSetLayouts = descriptorSetLayouts.data();

        checkResult(vkAllocateDescriptorSets(device, &allocateInfo, descriptorSets.data()));
        std::cout << "created descriptor sets" << std::endl;

        return descriptorSets;
    }

//    void Engine::createImage() {
//        VkImage image;
//        VkImageView imageView;
//
//        VkImageCreateInfo imageInfo{};
//        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//        //imageInfo.format
//        //imageInfo.extent
//        imageInfo.mipLevels = 0;
//        //imageInfo.arrayLayers
//        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
//        //imageInfo.usage
//        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//        //imageInfo.queueFamilyIndexCount
//        //imageInfo.pQueueFamilyIndices
//
//        checkResult(vkCreateImage(context->device, &imageInfo, nullptr, &image));
//
//        VkImageViewCreateInfo imageViewInfo{};
//        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//        imageViewInfo.image = image;
//        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//        //imageViewInfo.format
//        //imageViewInfo.components.r
//        //imageViewInfo.components.g
//        //imageViewInfo.components.b
//        //imageViewInfo.components.a
//        //imageViewInfo.subresourceRange.layerCount
//        //imageViewInfo.subresourceRange.baseArrayLayer
//        //imageViewInfo.subresourceRange.levelCount
//        //imageViewInfo.subresourceRange.baseMipLevel
//        //imageViewInfo.subresourceRange.aspectMask
//
//        checkResult(vkCreateImageView(context->device, &imageViewInfo, nullptr, &imageView));
//
//        VmaAllocator &a = context->allocator->allocator;
//
//        vkDestroyImageView(context->device, imageView, nullptr);
//        vkDestroyImage(context->device, image, nullptr);
//    }
}