#ifndef SPHERE_APPLICATION_HPP
#define SPHERE_APPLICATION_HPP

#include <window.hpp>
#include <device.hpp>
#include "scene.hpp"

namespace sphere {

    void printPosition(const glm::vec3 position) {
        std::cout << "x: " << position.x << ", "
                  << "y: " << position.y << ", "
                  << "z: " << position.z << '\n';
    }

    class Application {

    public:

        Application() :
            window("Application name", 200, 100),
            device(window) {

        }

        ~Application() {

        }

        // main loop
        void run() {
//            Scene scene{"test"};
//
//            Node test{"Test", &scene.rootNode};
//            Node child{"Child", &test};
//            Node child2{"Child 2", &child};
//            Node child3{"Child 3", &child};
//            Node grandChild{"GrandChild", &child3};
//
//            test.setLocalPosition({3.0f, 0, 0});
//            child.setLocalPosition({0, 1.0f, 32.0f});
//
//            scene.rootNode.print();
//
//            std::cout << "test position:\n";
//            sphere::printPosition(test.getPosition());
//            sphere::printPosition(test.getLocalPosition());
//
//            std::cout << "child position:\n";
//            sphere::printPosition(child.getPosition());
//            sphere::printPosition(child.getLocalPosition());
        }

    private:
        renderer::Window window;
        renderer::Device device;
    };

} // sphere

#endif //SPHERE_APPLICATION_HPP
