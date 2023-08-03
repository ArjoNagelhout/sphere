#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "device.hpp"
#include "swapchain.hpp"
#include "graphics_pipeline.hpp"

#include <iostream>
#include <memory>

namespace renderer {

    /*
     * The Renderer class ties together the different Vulkan objects and
     * is the main entry point for getting things onto the screen.
     */
    class Renderer {

    public:
        explicit Renderer(const std::string &applicationName, bool debug) {
            window = std::make_unique<Window>(applicationName, 600, 300, 200, 100);
            device = std::make_unique<Device>(*window, debug);
            swapchain = std::make_unique<Swapchain>(*window, *device);
            renderPass = std::make_unique<RenderPass>(*device, *swapchain);
            swapchain->createSwapchainFramebuffers(renderPass->getRenderPass());
            graphicsPipeline = std::make_unique<GraphicsPipeline>(*device, *swapchain, *renderPass);


            createCommandPool(*device, commandPool);
            createCommandBuffers(device->getDevice(), commandPool, MAX_FRAMES_IN_FLIGHT, commandBuffers);
            createSynchronizationPrimitives(device->getDevice(),
                                            imageAvailableSemaphores,
                                            renderFinishedSemaphores,
                                            inFlightFences,
                                            MAX_FRAMES_IN_FLIGHT);

            createVertexBuffer(*device, vertexBuffer);
        }

        ~Renderer() {

            vkDestroyBuffer(device->getDevice(), vertexBuffer, nullptr);

            // destroy synchronization primitives
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(device->getDevice(), imageAvailableSemaphores[i], nullptr);
                vkDestroySemaphore(device->getDevice(), renderFinishedSemaphores[i], nullptr);
                vkDestroyFence(device->getDevice(), inFlightFences[i], nullptr);
            }

            vkDestroyCommandPool(device->getDevice(), commandPool, nullptr);
        }

        void run() {
            GLFWwindow *glfwWindow = window->getWindow();

            //while (!glfwWindowShouldClose(glfwWindow)) {
                //glfwPollEvents();
                drawFrame(currentFrame,
                          MAX_FRAMES_IN_FLIGHT,
                          *device,
                          *swapchain,
                          *graphicsPipeline,
                          *renderPass,
                          commandBuffers,
                          imageAvailableSemaphores,
                          renderFinishedSemaphores,
                          inFlightFences,
                          vertexBuffer);
            //}
            vkDeviceWaitIdle(device->getDevice());
        }

    private:
        std::unique_ptr<Window> window;
        std::unique_ptr<Device> device;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;

        const int MAX_FRAMES_IN_FLIGHT = 2;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        uint32_t currentFrame = 0;

        VkBuffer vertexBuffer;

        static void createCommandPool(Device &device,
                                      VkCommandPool &commandPool) {
            QueueFamiliesData data = device.getQueueFamiliesData();

            VkCommandPoolCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            createInfo.queueFamilyIndex = data.graphicsQueueFamilyData->index;

            VkResult result = vkCreateCommandPool(device.getDevice(), &createInfo, nullptr, &commandPool);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create command pool") + string_VkResult(result));
            }

            std::cout << "created command pool" << std::endl;
        }

        static void createCommandBuffers(const VkDevice &device, VkCommandPool &commandPool, int maxFramesInFlight,
                                         std::vector<VkCommandBuffer> &commandBuffers) {
            commandBuffers.resize(maxFramesInFlight);

            VkCommandBufferAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocateInfo.commandPool = commandPool;
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

            VkResult result = vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data());

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to allocate command buffers: ") + string_VkResult(result));
            }

            std::cout << "created command buffers" << std::endl;
        }

        static void createSynchronizationPrimitives(
                const VkDevice &device,
                std::vector<VkSemaphore> &imageAvailableSemaphores,
                std::vector<VkSemaphore> &renderFinishedSemaphores,
                std::vector<VkFence> &inFlightFences,
                int maxFramesInFlight) {
            imageAvailableSemaphores.resize(maxFramesInFlight);
            renderFinishedSemaphores.resize(maxFramesInFlight);
            inFlightFences.resize(maxFramesInFlight);

            VkSemaphoreCreateInfo semaphoreCreateInfo{};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < maxFramesInFlight; i++) {
                VkResult createImageAvailableSemaphoresResult =
                        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]);
                if (createImageAvailableSemaphoresResult != VK_SUCCESS) {
                    throw std::runtime_error(std::string("failed to create image available semaphores") +
                                             string_VkResult(createImageAvailableSemaphoresResult));
                }

                VkResult createRenderFinishedSemaphoresResult =
                        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]);
                if (createRenderFinishedSemaphoresResult != VK_SUCCESS) {
                    throw std::runtime_error(std::string("failed to create render finished semaphores") +
                                             string_VkResult(createRenderFinishedSemaphoresResult));
                }

                VkResult createInFlightFencesResult =
                        vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]);
                if (createInFlightFencesResult != VK_SUCCESS) {
                    throw std::runtime_error(std::string("failed to create in flight fences") +
                                             string_VkResult(createInFlightFencesResult));
                }
            }

            std::cout << "created synchronization primitives" << std::endl;
        }

        static void createVertexBuffer(Device &device, VkBuffer &vertexBuffer) {

            PhysicalDeviceData physicalDeviceData = device.getPhysicalDeviceData();

            uint32_t vertexStride = physicalDeviceData.minVertexInputBindingStrideAlignment;
            uint32_t vertexCount = 3;

            VkBufferCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createInfo.size = vertexStride * vertexCount; // size in bytes, should be greater than zero
            createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;

            VkResult result = vkCreateBuffer(device.getDevice(), &createInfo, nullptr, &vertexBuffer);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create vertex buffer") + string_VkResult(result));
            }

            std::cout << "created vertex buffer" << std::endl;
        }

        static void recordCommandBuffer(
                VkCommandBuffer &commandBuffer,
                uint32_t imageIndex,
                Swapchain &swapchain,
                GraphicsPipeline &graphicsPipeline,
                RenderPass &renderPass,
                VkBuffer &vertexBuffer) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(
                        std::string("failed to begin recording command buffer") + string_VkResult(result));
            }

            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

            VkExtent2D extent = swapchain.getSwapchainExtent();

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass.getRenderPass();
            renderPassInfo.framebuffer = swapchain.getSwapchainFramebuffers()[imageIndex];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapchain.getSwapchainExtent();
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getGraphicsPipeline());

            VkViewport viewport{
                    .x = 0.0f,
                    .y = 0.0f,
                    .width = static_cast<float>(extent.width),
                    .height = static_cast<float>(extent.height),
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f};
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{
                    .offset = {0, 0},
                    .extent = extent};
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

            vkCmdEndRenderPass(commandBuffer);

            VkResult endBufferResult = vkEndCommandBuffer(commandBuffer);
            if (endBufferResult != VK_SUCCESS) {
                throw std::runtime_error(
                        std::string("failed to record command buffer") + string_VkResult(endBufferResult));
            }
        }

        static void drawFrame(uint32_t &currentFrame,
                              int maxFramesInFlight,
                              Device &device,
                              Swapchain &swapchain,
                              GraphicsPipeline &graphicsPipeline,
                              RenderPass &renderPass,
                              std::vector<VkCommandBuffer> &commandBuffers,
                              std::vector<VkSemaphore> &imageAvailableSemaphores,
                              std::vector<VkSemaphore> &renderFinishedSemaphores,
                              std::vector<VkFence> &inFlightFences,
                              VkBuffer &vertexBuffer) {

            vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]);

            uint32_t imageIndex;
            VkResult acquireNextImageResult = vkAcquireNextImageKHR(device.getDevice(), swapchain.getSwapchain(),
                                                                    UINT64_MAX, imageAvailableSemaphores[currentFrame],
                                                                    VK_NULL_HANDLE, &imageIndex);

            switch (acquireNextImageResult) {
                case VK_SUCCESS:
                case VK_SUBOPTIMAL_KHR:
                    break;
                default:
                    throw std::runtime_error(
                            std::string("failed to acquire swapchain image") + string_VkResult(acquireNextImageResult));
            }

            vkResetCommandBuffer(commandBuffers[currentFrame], 0);
            recordCommandBuffer(commandBuffers[currentFrame], imageIndex, swapchain, graphicsPipeline, renderPass, vertexBuffer);

            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            VkResult queueSubmitResult = vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo,
                                                       inFlightFences[currentFrame]);

            if (queueSubmitResult != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to submit queue") + string_VkResult(queueSubmitResult));
            }

            VkSwapchainKHR swapchains[] = {swapchain.getSwapchain()};

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapchains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            VkResult queuePresentResult = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

            switch (queuePresentResult) {
                case VK_SUBOPTIMAL_KHR:
                    break;
                case VK_SUCCESS:
                    break;
                default:
                    throw std::runtime_error(
                            std::string("failed to present swap chain image") + string_VkResult(queuePresentResult));
            }

            currentFrame = (currentFrame + 1) % maxFramesInFlight;
        }
    };
}

#endif //SPHERE_RENDERER_HPP
