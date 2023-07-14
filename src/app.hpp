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

            Node test{"Test"};
            Node child{"Child"};
            Node grandChild{"GrandChild"};

            child.add(grandChild);
            test.add(child);
            test.add(child);
            test.add(child);
            scene.rootNode.add(test);

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
