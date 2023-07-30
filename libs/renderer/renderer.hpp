#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "device.hpp"
#include "swapchain.hpp"

namespace renderer {

    class Renderer {

    public:
        explicit Renderer() :
                window("Application name", 200, 100),
                device(window),
                swapchain(window, device) {

        }

        ~Renderer() {

        }

    private:
        Window window;
        Device device;
        Swapchain swapchain;
    };
}

#endif //SPHERE_RENDERER_HPP
