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
        }

        ~Renderer() = default;

        void run() {
            GLFWwindow *glfwWindow = window->getWindow();

            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
                drawFrame();
            }
            vkDeviceWaitIdle(device->getDevice());
        }

    private:
        std::unique_ptr<Window> window;
        std::unique_ptr<Device> device;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;

        static void drawFrame() {

        }
    };
}

#endif //SPHERE_RENDERER_HPP
