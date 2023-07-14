#ifndef SPHERE_APP_HPP
#define SPHERE_APP_HPP

#include <renderer.hpp>
#include "scene.hpp"

namespace sphere {

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

            scene.rootNode.add(&test);
            child.add(&grandChild);
            test.add(&child);
            test.add(&child2);
            test.add(&child3);

            scene.rootNode.print();
        }

        // cleanup resources
        void cleanUp() {

        }

    private:
        renderer::Renderer renderer;
    };

} // sphere

#endif //SPHERE_APP_HPP
