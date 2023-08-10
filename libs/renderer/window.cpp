#include "window.h"

namespace renderer {

    Window::Window(std::string title, int width, int height, int minWidth, int minHeight) {

        // create window
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindow = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        glfwSetWindowSizeLimits(glfwWindow, minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

        std::cout << "created window" << std::endl;
    }

    Window::~Window() {
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }
}