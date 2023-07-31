#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "device.hpp"
#include "swapchain.hpp"

namespace renderer {

    /*
     * The Renderer class ties together the different Vulkan objects and
     * is the main entry point for getting things onto the screen.
     */
    class Renderer {

    public:
        explicit Renderer(const std::string &applicationName) :
                window(applicationName, 600, 300, 200, 100),
                device(window),
                swapchain(window, device) {
        }

        ~Renderer() {

        }

        void run() {
            GLFWwindow *glfwWindow = window.getWindow();

            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
            }

        }

    private:
        Window window;
        Device device;
        Swapchain swapchain;
    };
}

#endif //SPHERE_RENDERER_HPP
