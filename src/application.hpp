#ifndef SPHERE_APPLICATION_HPP
#define SPHERE_APPLICATION_HPP

#include <window.hpp>
#include <renderer.hpp>

#include "scene.hpp"

#include <unordered_map>

#include <glm/glm.hpp>

namespace sphere {

    class Application {

    public:

        Application(const std::string &applicationName) {
            window = std::make_unique<renderer::Window>(applicationName, 600, 300, 200, 100);
            renderer = std::make_unique<renderer::Renderer>(*window, true);

            GLFWwindow *glfwWindow = window->getWindow();
            glfwSetWindowUserPointer(glfwWindow, this);
            glfwSetKeyCallback(glfwWindow, keyCallback);
        }

        ~Application() {

        }

        // main loop
        void run() {
            GLFWwindow *glfwWindow = window->getWindow();
            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
                updateCameraPosition();
                renderer->update();
            }
        }

    private:
        std::unique_ptr<renderer::Window> window;
        std::unique_ptr<renderer::Renderer> renderer;

        bool keys[GLFW_KEY_LAST];

        void updateCameraPosition() {
            glm::vec3 &cameraPosition = renderer->cameraPosition;

            int forward = (int)keys[GLFW_KEY_W] - (int)keys[GLFW_KEY_S];
            int right = (int)keys[GLFW_KEY_D] - (int)keys[GLFW_KEY_A];
            int up = (int)keys[GLFW_KEY_E] - (int)keys[GLFW_KEY_Q];

            cameraPosition.x += right;
            cameraPosition.y += up;
            cameraPosition.z += forward;
        }

        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            Application *application = static_cast<Application*>(glfwGetWindowUserPointer(window));

            // ensure it is a key
            if (key < 0 || key > GLFW_KEY_LAST) {
                return;
            }

            bool &isPressed = application->keys[key];

            if (action == GLFW_PRESS) {
                isPressed = true;
            } else if (action == GLFW_RELEASE) {
                isPressed = false;
            }
        }
    };
}

#endif //SPHERE_APPLICATION_HPP
