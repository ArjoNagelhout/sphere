#ifndef SPHERE_APP_HPP
#define SPHERE_APP_HPP

#include <renderer.hpp>
#include "scene.hpp"

namespace sphere {

    void printPosition(const glm::vec3 position) {
        std::cout << "x: " << position.x << ", "
                  << "y: " << position.y << ", "
                  << "z: " << position.z << '\n';
    }

    class App {

    public:
        // e.g. Initialize all resources
        void initialize() {

        }

        // main loop
        static void run() {
            Scene scene{"test"};

            Node test {"Test", &scene.rootNode};
            Node child {"Child", &test};
            Node child2 {"Child 2", &child};
            Node child3 {"Child 3", &child};
            Node grandChild {"GrandChild", &child3};

            test.setLocalPosition({3.0f, 0, 0});
            child.setLocalPosition({0, 2.0f, 0});

            scene.rootNode.print();

            std::cout << "test position:\n";
            sphere::printPosition(test.getPosition());
            sphere::printPosition(test.getLocalPosition());

            std::cout << "child position:\n";
            sphere::printPosition(child.getPosition());
            sphere::printPosition(child.getLocalPosition());
        }

        // cleanup resources
        void cleanUp() {

        }

    private:
        renderer::Renderer renderer;
    };

} // sphere

#endif //SPHERE_APP_HPP
