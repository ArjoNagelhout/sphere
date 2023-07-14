#ifndef SPHERE_APP_HPP
#define SPHERE_APP_HPP

#include <renderer.hpp>

namespace sphere {

    class App {

    public:
        // e.g. Initialize all resources
        void initialize();

        // main loop
        void run();

        // cleanup resources
        void cleanUp();

    private:
        renderer::Renderer renderer;
    };

} // sphere

#endif //SPHERE_APP_HPP
