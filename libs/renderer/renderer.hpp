#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "device.hpp"
#include "swapchain.hpp"
#include "graphics_pipeline.hpp"

#include <iostream>

namespace renderer {

    /*
     * The Renderer class ties together the different Vulkan objects and
     * is the main entry point for getting things onto the screen.
     */
    class Renderer {

    public:
        explicit Renderer(const std::string &applicationName, bool debug) :
                window(applicationName, 600, 300, 200, 100),
                device(window, debug),
                swapchain(window, device),
                renderPass(device, swapchain),
                graphicsPipeline(device, swapchain, renderPass) {

            createFrameBuffers(device.getDevice(),
                               renderPass.getRenderPass(),
                               swapchain.getSwapchainImageViews(),
                               swapchain.getSwapchainExtent(),
                               framebuffers);
        }

        ~Renderer() {
            vkDestroyFramebuffer()
        }

        void run() {
            GLFWwindow *glfwWindow = window.getWindow();

            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
                drawFrame();
            }
            vkDeviceWaitIdle(device.getDevice());
        }

    private:
        Window window;
        Device device;
        Swapchain swapchain;
        RenderPass renderPass;
        GraphicsPipeline graphicsPipeline;

        std::vector<VkFramebuffer> framebuffers;

        /*
         * Dependent on both render pass (stored now in GraphicsPipeline)
         * and swapchain (stored in Swapchain), so we move it here for now.
         */
        static void createFrameBuffers(const VkDevice &device,
                                       const VkRenderPass &renderPass,
                                       const std::vector<VkImageView> &swapchainImageViews,
                                       const VkExtent2D &swapchainExtent,
                                       std::vector<VkFramebuffer> swapchainFramebuffers) {
            swapchainFramebuffers.resize(swapchainImageViews.size());

            for (size_t i = 0; i < swapchainImageViews.size(); i++) {
                std::vector<VkImageView> attachments{
                        swapchainImageViews[i]
                };

                VkFramebufferCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                createInfo.renderPass = renderPass;
                createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                createInfo.pAttachments = attachments.data();
                createInfo.width = swapchainExtent.width;
                createInfo.height = swapchainExtent.height;
                createInfo.layers = 1;

                VkResult result = vkCreateFramebuffer(device, &createInfo, nullptr, &swapchainFramebuffers[i]);

                if (result != VK_SUCCESS) {
                    throw std::runtime_error(std::string("failed to create frame buffer: ") + string_VkResult(result));
                }
            }

            std::cout << "created frame buffers" << std::endl;
        }

        static void drawFrame() {

        }
    };
}

#endif //SPHERE_RENDERER_HPP
