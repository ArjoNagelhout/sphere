#ifndef SPHERE_APPLICATION_H
#define SPHERE_APPLICATION_H

#include <window.h>
#include <vulkan_context.h>
#include <engine.h>

#include <unordered_map>

#include <glm/glm.hpp>

namespace sphere {

    class Application {

    public:

        explicit Application(const std::string &applicationName) {

            window = std::make_unique<engine::Window>(applicationName, 1000, 600, 200, 100);

            engine::EngineConfiguration configuration{
                .window = window->glfwWindow,
                .debug = true,
                .applicationName = "Sphere",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0)
            };

            engine = std::make_unique<engine::Engine>(configuration);

            //glfwSetWindowUserPointer(glfwWindow, this);
            //glfwSetKeyCallback(glfwWindow, keyCallback);
        }

        ~Application() {

        }

        // main loop
        void run() {
            GLFWwindow *glfwWindow = window->glfwWindow;
            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
                //updateCameraPosition();
                engine->render();
            }
        }

    private:
        std::unique_ptr<engine::Window> window;
        std::unique_ptr<engine::Engine> engine;

        //bool keys[GLFW_KEY_LAST];

//        void updateCameraPosition() {
//            glm::vec3 &cameraPosition = renderer->cameraPosition;
//
//            int forward = (int)keys[GLFW_KEY_W] - (int)keys[GLFW_KEY_S];
//            int right = (int)keys[GLFW_KEY_D] - (int)keys[GLFW_KEY_A];
//            int up = (int)keys[GLFW_KEY_E] - (int)keys[GLFW_KEY_Q];
//
//            cameraPosition.x += right;
//            cameraPosition.y += up;
//            cameraPosition.z += forward;
//        }

//        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//            Application *application = static_cast<Application*>(glfwGetWindowUserPointer(window));
//
//            // ensure it is a key
//            if (key < 0 || key > GLFW_KEY_LAST) {
//                return;
//            }
//
//            bool &isPressed = application->keys[key];
//
//            if (action == GLFW_PRESS) {
//                isPressed = true;
//            } else if (action == GLFW_RELEASE) {
//                isPressed = false;
//            }
//        }
    };
}

#endif //SPHERE_APPLICATION_H
