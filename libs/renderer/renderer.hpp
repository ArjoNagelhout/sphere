#include <vulkan/vulkan.hpp>

namespace renderer {

    // vulkan renderer
    class Renderer {

    public:
        void render();

    private:
        vk::Device device;
		Context context;
    };
}