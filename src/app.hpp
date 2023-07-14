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

            Node test {"Test"};
            Node child {"Child"};
            Node child2 {"Child 2"};
            Node child3 {"Child 3"};
            Node grandChild {"GrandChild"};

            test.setLocalPosition({3.2323f, 0, 0});
            child.setLocalPosition({0, 2.1113f, 0});

            scene.rootNode.addChild(&test);
            child.addChild(&grandChild);
            test.addChild(&child);
            test.addChild(&child2);
            test.addChild(&child3);

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
