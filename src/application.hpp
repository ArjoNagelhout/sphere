#ifndef SPHERE_APPLICATION_HPP
#define SPHERE_APPLICATION_HPP

#include <window.hpp>
#include <renderer.hpp>

#include "scene.hpp"

namespace sphere {

    class Application {

    public:

        Application(const std::string &applicationName) {
            window = std::make_unique<renderer::Window>(applicationName, 600, 300, 200, 100);
            renderer = std::make_unique<renderer::Renderer>(*window, true);
        }

        ~Application() {

        }

        // main loop
        void run() {

            GLFWwindow *glfwWindow = window->getWindow();

            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
                renderer->update();
            }
        }

    private:
        std::unique_ptr<renderer::Window> window;
        std::unique_ptr<renderer::Renderer> renderer;
    };

}

#endif //SPHERE_APPLICATION_HPP
