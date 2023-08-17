#ifndef SPHERE_APPLICATION_H
#define SPHERE_APPLICATION_H

#include "window.h"
#include <engine.h>
#include <editor.h>

#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace sphere {

    class Application {

    public:

        explicit Application(const std::string &applicationName) {

            window = std::make_unique<engine::Window>(applicationName, 1250, 1250, 200, 100);

            engine::EngineConfiguration configuration{
                    .window = window->glfwWindow,
                    .debug = true,
                    .applicationName = "Sphere",
                    .applicationVersion = VK_MAKE_VERSION(1, 0, 0)
            };

            engine = std::make_unique<engine::Engine>(configuration);
            editor = std::make_unique<editor::Editor>();

            engine->renderImgui = [&]() {
                editor->render();
            };

            glfwSetWindowUserPointer(window->glfwWindow, this);
            glfwSetKeyCallback(window->glfwWindow, keyCallback);
        }

        ~Application() = default;

        // main loop
        void run() {
            GLFWwindow *glfwWindow = window->glfwWindow;
            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
                updateCameraPosition();
                engine->render();
            }
        }

    private:
        std::unique_ptr<engine::Window> window;
        std::unique_ptr<engine::Engine> engine;
        std::unique_ptr<editor::Editor> editor;

        bool keys[GLFW_KEY_LAST + 1];
        const float speed = 0.1f;
        const float turnSpeed = 0.05f;

        void updateCameraPosition() {
            glm::vec3 &cameraPosition = engine->camera->position;
            glm::quat &cameraRotation = engine->camera->rotation;

            int forward = (int) keys[GLFW_KEY_W] - (int) keys[GLFW_KEY_S];
            int right = (int) keys[GLFW_KEY_D] - (int) keys[GLFW_KEY_A];
            int up = (int) keys[GLFW_KEY_E] - (int) keys[GLFW_KEY_Q];

            int turnRight = (int) keys[GLFW_KEY_RIGHT] - (int) keys[GLFW_KEY_LEFT];
            int turnUp = (int) keys[GLFW_KEY_UP] - (int) keys[GLFW_KEY_DOWN];

            // need to get forward vector, we do this by getting the rotation

            glm::vec3 deltaPosition{(float) right * speed, -(float) up * speed, -(float) forward * speed};
            // now rotate the position

            cameraPosition += cameraRotation * deltaPosition;

            glm::vec3 rightRelativeToCamera = cameraRotation * glm::vec3(1, 0, 0);

            glm::quat deltaHorizontalRotation = glm::angleAxis((float) turnRight * turnSpeed, glm::vec3{0, 1, 0});
            glm::quat deltaVerticalRotation = glm::angleAxis(-(float) turnUp * turnSpeed, rightRelativeToCamera);

            //printRotation("cameraRotation", cameraRotation);
            //printRotation("deltaRotation", deltaRotation);
            cameraRotation = deltaVerticalRotation * deltaHorizontalRotation * cameraRotation;
//            printRotation("cameraRotationAfter", cameraRotation);


            //cameraRotation.x += (float)turnUp * turnSpeed;
            //cameraRotation.z += (float)turnRight * turnSpeed;
        }

        static void printRotation(const std::string &str, const glm::quat &rot) {
            std::cout << str
                      << ": x: " << rot.x
                      << ", y: " << rot.y
                      << ", z: " << rot.z
                      << ", w: " << rot.w << std::endl;
        }

        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            Application *application = static_cast<Application *>(glfwGetWindowUserPointer(window));

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

#endif //SPHERE_APPLICATION_H
