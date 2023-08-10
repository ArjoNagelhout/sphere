#include "renderer.hpp"

namespace renderer {

    FrameData::FrameData(VkCommandBuffer &commandBuffer, VkDescriptorSet &descriptorSet)
            : commandBuffer(commandBuffer), descriptorSet(descriptorSet) {
        Engine &engine = getEngine();

        // create synchronization primitives
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        checkResult(vkCreateSemaphore(engine.device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
        checkResult(vkCreateSemaphore(engine.device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));
        checkResult(vkCreateFence(engine.device, &fenceCreateInfo, nullptr, &inFlightFence));

        std::cout << "created frame data" << std::endl;
    }

    FrameData::~FrameData() {
        Engine &engine = getEngine();

        vkDestroySemaphore(engine.device, imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(engine.device, renderFinishedSemaphore, nullptr);
        vkDestroyFence(engine.device, inFlightFence, nullptr);
    }

    void FrameData::updateDescriptorSet(VkBuffer &buffer, VkDescriptorType descriptorType) const {
        Engine &engine = getEngine();

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

        vkUpdateDescriptorSets(engine.device, 1, &writeDescriptorSet, 0, nullptr);
    }

    Renderer::Renderer(RendererConfiguration &configuration) {

        std::cout << "created renderer" << std::endl;

        VulkanConfiguration vulkanConfiguration{
                .window = configuration.window,

                .engineName = ENGINE_NAME,
                .applicationName = configuration.applicationName,
                .engineVersion = ENGINE_VERSION,
                .applicationVersion = configuration.applicationVersion,

                .debug = configuration.debug,

                .preferredSurfaceFormats = {},
                .requiredInstanceExtensions = {},
                .requiredInstanceLayers = {},
                .requiredDeviceExtensions = {}
        };

        initializeEngine(vulkanConfiguration);

        swapchain = std::make_unique<Swapchain>(preferredSurfaceFormats);
        renderPass = std::make_unique<RenderPass>(swapchain->surfaceFormat.format);
        graphicsPipeline = std::make_unique<GraphicsPipeline>(*swapchain, *renderPass);
        memoryAllocator = std::make_unique<MemoryAllocator>();

        memoryAllocator->createBuffer<CameraData>(cameraDataBuffer,
                                                  cameraDataBufferAllocation,
                                                  cameraData,
                                                  sizeof(cameraData),
                                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

        createCommandPool();
        createDescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        std::vector<VkCommandBuffer> commandBuffers = allocateCommandBuffers();
        std::vector<VkDescriptorSet> descriptorSets = allocateDescriptorSets(graphicsPipeline->descriptorSetLayout);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            FrameData frameData{commandBuffers[i], descriptorSets[i]};
            frames.push_back(frameData);
            frameData.updateDescriptorSet(cameraDataBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        }

        memoryAllocator->createBuffer<VertexAttributes>(vertexBuffer,
                                                         vertexBufferAllocation,
                                                         *vertices.data(),
                                                         vertices.size() * sizeof(vertices[0]),
                                                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        memoryAllocator->createBuffer<uint32_t>(indexBuffer,
                                                indexBufferAllocation,
                                                *indices.data(),
                                                indices.size() * sizeof(indices[0]),
                                                VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    Renderer::~Renderer() {
        Engine &engine = getEngine();

        vkDestroyCommandPool(engine.device, commandPool, nullptr);
        vkDestroyDescriptorPool(engine.device, descriptorPool, nullptr);

        memoryAllocator.reset();
        graphicsPipeline.reset();
        renderPass.reset();
        swapchain.reset();

        destroyEngine();

        std::cout << "destroyed renderer" << std::endl;
    }

    void Renderer::recordCommandBuffer(FrameData frameData, VkFramebuffer framebuffer) {
        VkCommandBuffer &cb = frameData.commandBuffer;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        checkResult(vkBeginCommandBuffer(cb, &beginInfo));

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkExtent2D extent = swapchain->extent;

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass->renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cb, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindDescriptorSets(cb,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipeline->graphicsPipelineLayout,
                                0,
                                1,
                                &frameData.descriptorSet,
                                0,
                                nullptr);
        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);

        VkViewport viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(extent.width),
                .height = static_cast<float>(extent.height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f};
        vkCmdSetViewport(cb, 0, 1, &viewport);

        VkRect2D scissor{
                .offset = {0, 0},
                .extent = extent};
        vkCmdSetScissor(cb, 0, 1, &scissor);

        VkDeviceSize vertexBufferOffset = 0;
        vkCmdBindIndexBuffer(cb, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindVertexBuffers(cb, 0, 1, &vertexBuffer, &vertexBufferOffset);
        vkCmdDrawIndexed(cb, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
        vkCmdEndRenderPass(cb);
        checkResult(vkEndCommandBuffer(cb));
    }

    void Renderer::render() {
        drawFrame(frames[currentFrameIndex]);
        currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::drawFrame(FrameData frameData) {
        Engine &engine = getEngine();
        VkResult result;

        vkWaitForFences(engine.device, 1, &frameData.inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(engine.device, 1, &frameData.inFlightFence);

        uint32_t imageIndex;
        result = vkAcquireNextImageKHR(engine.device,
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

        checkResult(vkQueueSubmit(engine.graphicsQueue, 1, &submitInfo, frameData.inFlightFence));

        VkSwapchainKHR swapchains[] = {swapchain->swapchain};

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        result = vkQueuePresentKHR(engine.presentQueue, &presentInfo);
        switch (result) {
            case VK_SUCCESS:
            case VK_SUBOPTIMAL_KHR:
                break;
            default:
                throw std::runtime_error(
                        std::string("failed to present swap chain image") + string_VkResult(result));
        }
    }

    void Renderer::createCommandPool() {
        Engine &engine = getEngine();

        QueueFamiliesData data = engine.queueFamiliesData;

        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = data.graphicsQueueFamilyData->index;

        checkResult(vkCreateCommandPool(engine.device, &createInfo, nullptr, &commandPool));
        std::cout << "created command pool" << std::endl;
    }

    void Renderer::createDescriptorPool(const VkDescriptorType &descriptorType) {
        Engine &engine = getEngine();

        VkDescriptorPoolSize poolSize{
                .type = descriptorType,
                .descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        };

        VkDescriptorPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        createInfo.poolSizeCount = 1;
        createInfo.pPoolSizes = &poolSize;

        checkResult(vkCreateDescriptorPool(engine.device, &createInfo, nullptr, &descriptorPool));
        std::cout << "created descriptor pool" << std::endl;
    }

    std::vector<VkCommandBuffer> Renderer::allocateCommandBuffers() {
        Engine &engine = getEngine();
        std::vector<VkCommandBuffer> commandBuffers(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        checkResult(vkAllocateCommandBuffers(engine.device, &allocateInfo, commandBuffers.data()));
        std::cout << "created command buffers" << std::endl;

        return commandBuffers;
    }

    std::vector<VkDescriptorSet> Renderer::allocateDescriptorSets(VkDescriptorSetLayout &descriptorSetLayout) {
        Engine &engine = getEngine();
        std::vector<VkDescriptorSet> descriptorSets(MAX_FRAMES_IN_FLIGHT);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSets.size());
        allocateInfo.pSetLayouts = descriptorSetLayouts.data();

        checkResult(vkAllocateDescriptorSets(engine.device, &allocateInfo, descriptorSets.data()));
        std::cout << "created descriptor sets" << std::endl;

        return descriptorSets;
    }
}