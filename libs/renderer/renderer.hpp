#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "device.hpp"
#include "swapchain.hpp"
#include "graphics_pipeline.hpp"

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
                graphicsPipeline(device, swapchain) {
            std::cout << "renderer" << std::endl;
        }

        ~Renderer() {

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
        GraphicsPipeline graphicsPipeline;

        void drawFrame() {

        }
    };
}

#endif //SPHERE_RENDERER_HPP
