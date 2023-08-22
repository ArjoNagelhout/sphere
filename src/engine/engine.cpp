#include "engine.h"

namespace engine {

    Engine *engine;

    void FrameData::initialize() {
        // create synchronization primitives
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        renderer::checkResult(vkCreateSemaphore(renderer::context->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
        renderer::checkResult(vkCreateSemaphore(renderer::context->device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));
        renderer::checkResult(vkCreateFence(renderer::context->device, &fenceCreateInfo, nullptr, &inFlightFence));

        std::cout << "created frame data" << std::endl;
    }

    void FrameData::destroy() const {
        vkDestroySemaphore(renderer::context->device, imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(renderer::context->device, renderFinishedSemaphore, nullptr);
        vkDestroyFence(renderer::context->device, inFlightFence, nullptr);

//        std::cout << "destroyed frame data" << std::endl;
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

        renderer::VulkanConfiguration configuration{
                .window = engineConfiguration.window,

                .engineName = ENGINE_NAME,
                .applicationName = engineConfiguration.applicationName,
                .engineVersion = ENGINE_VERSION,
                .applicationVersion = engineConfiguration.applicationVersion,

                .debug = engineConfiguration.debug,

                .preferredSurfaceFormats = {},
                .requiredInstanceExtensions = {},
                .requiredInstanceLayers = {},
                .requiredDeviceExtensions = requiredDeviceExtensions,
        };
        context = std::make_unique<renderer::VulkanContext>(configuration);
        swapchain = std::make_unique<renderer::Swapchain>(renderer::preferredSurfaceFormats);
        renderPass = std::make_unique<renderer::RenderPass>(swapchain->surfaceFormat.format, depthImageFormat);
        descriptorSetBuilder = std::make_unique<renderer::DescriptorSetBuilder>();
        pipelineBuilder = std::make_unique<renderer::PipelineBuilder>(*swapchain);

        glfwSetFramebufferSizeCallback(configuration.window, framebufferResizeCallback);

        createDepthImage();
        swapchain->createFramebuffers(renderPass->renderPass, depthImageView);
        camera = std::make_unique<renderer::Camera>(*swapchain);

        commandPool = renderer::createCommandPool();

        std::vector<VkCommandBuffer> commandBuffers = renderer::createCommandBuffers(commandPool, MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            FrameData frameData{
                    .commandBuffer = commandBuffers[i],
            };
            frameData.initialize();
            frames.push_back(frameData);
        }

        scene = std::make_unique<renderer::Scene>(renderPass->renderPass);
        // bind the camera buffer with the materials
        for (auto const &material : scene->materials) {
            renderer::bindBuffer(material->descriptorSet, camera->cameraDataBuffer.buffer, 0);
        }
    }

    Engine::~Engine() {
        vkDeviceWaitIdle(context->device);

        for (auto const &frameData: frames) {
            frameData.destroy();
        }

        vkDestroyCommandPool(context->device, commandPool, nullptr);

        // destroy depth image
        vkDestroyImageView(context->device, depthImageView, nullptr);
        vmaDestroyImage(context->allocator, depthImage, depthImageAllocation);

        camera.reset();

        scene.reset();
        pipelineBuilder.reset();
        descriptorSetBuilder.reset();
        renderPass.reset();
        swapchain.reset();
        context.reset();

        std::cout << "destroyed engine" << std::endl;
    }

    void Engine::render() {
//        // render imgui
//        {
//            ImGui_ImplVulkan_NewFrame();
//            ImGui_ImplGlfw_NewFrame();
//            ImGui::NewFrame();
//            renderImgui(); // std::function call
//            ImGui::Render();
//        }
        camera->updateCameraData();
        scene->update();
        drawFrame();
    }

    void Engine::drawFrame() {
        const FrameData &frameData = frames[currentFrameIndex];
        VkResult result;
        vkWaitForFences(context->device, 1, &frameData.inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        result = vkAcquireNextImageKHR(context->device,
                                       swapchain->swapchain,
                                       UINT64_MAX,
                                       frameData.imageAvailableSemaphore,
                                       VK_NULL_HANDLE,
                                       &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

        } else {
            renderer::checkResult(result);
        }

        vkResetFences(context->device, 1, &frameData.inFlightFence);

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
        renderer::checkResult(vkQueueSubmit(context->graphicsQueue, 1, &submitInfo, frameData.inFlightFence));

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
        result = vkQueuePresentKHR(context->presentQueue, &presentInfo);
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
            swapchain->recreate();
            framebufferResized = false;
        } else {
            renderer::checkResult(result);
        }
        currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Engine::recordCommandBuffer(const FrameData &frameData, const VkFramebuffer &framebuffer) {
        const VkCommandBuffer &cmd = frameData.commandBuffer;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        renderer::checkResult(vkBeginCommandBuffer(cmd, &beginInfo));

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

        for (const auto &object: scene->objects) {
            // bind the pipeline
            renderer::PipelineData *pipelineData = object->material.shader.pipelineData;

            // should bind descriptor sets that are owned by either the material or shader.
            // shader has layout, material has descriptor sets themselves.
            vkCmdBindDescriptorSets(cmd,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineData->pipelineLayout,
                                    0,
                                    1,//static_cast<uint32_t>(object->material.descriptorSets.size()),
                                    &object->material.descriptorSet,//object->material.descriptorSets.data(),
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
            vkCmdBindIndexBuffer(cmd, object->mesh.indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindVertexBuffers(cmd, 0, 1, &(object->mesh.vertexBuffer->buffer), &vertexBufferOffset);
            vkCmdDrawIndexed(cmd, static_cast<uint32_t>(object->mesh.indices.size()), 1, 0, 0, 0);
        }

        // imgui
//        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

        vkCmdEndRenderPass(cmd);
        renderer::checkResult(vkEndCommandBuffer(cmd));
    }

    void Engine::createDepthImage() {
        VkExtent3D extent = renderer::toExtent3D(swapchain->extent);
        VkImageCreateInfo imageInfo = renderer::vk_create::image(depthImageFormat, extent,
                                                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        VmaAllocationCreateInfo allocationInfo{
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                .usage = VMA_MEMORY_USAGE_GPU_ONLY,
                .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };
        renderer::checkResult(vmaCreateImage(context->allocator,
                                   &imageInfo, &allocationInfo,
                                   &depthImage, &depthImageAllocation, nullptr));

        VkImageViewCreateInfo imageViewInfo = renderer::vk_create::imageView(depthImage, depthImageFormat,
                                                                   VK_IMAGE_ASPECT_DEPTH_BIT);
        renderer::checkResult(vkCreateImageView(context->device, &imageViewInfo, nullptr, &depthImageView));

        std::cout << "created depth image" << std::endl;
    }
}