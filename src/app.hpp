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
        void run() {
            Scene scene{"test"};
            std::cout << scene.GetName() << std::endl;
        }

        // cleanup resources
        void cleanUp() {

        }

    private:
        renderer::Renderer renderer;
    };

} // sphere

#endif //SPHERE_APP_HPP
