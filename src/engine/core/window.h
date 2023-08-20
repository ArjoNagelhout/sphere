#ifndef SPHERE_WINDOW_H
#define SPHERE_WINDOW_H

#include "includes.h"

namespace engine {

    class Window {

    public:
        explicit Window(std::string title,
                        int width,
                        int height,
                        int minWidth = GLFW_DONT_CARE,
                        int minHeight = GLFW_DONT_CARE);
        ~Window();
        GLFWwindow *glfwWindow;
    };
}

#endif //SPHERE_WINDOW_H
