#ifndef SPHERE_WINDOW_HPP
#define SPHERE_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace renderer {

    class Window {

    public:
        Window(char *name,
               uint32_t width,
               uint32_t height,
               uint32_t minWidth = GLFW_DONT_CARE,
               uint32_t minHeight = GLFW_DONT_CARE) {

            // create window
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            window = glfwCreateWindow(width, height, name, nullptr, nullptr);
            glfwSetWindowSizeLimits(window, minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
        }

        ~Window() {
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        GLFWwindow *getWindow() {
            return window;
        }

    private:
        GLFWwindow *window;
    };
}

#endif //SPHERE_WINDOW_HPP
