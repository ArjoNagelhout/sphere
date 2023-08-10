#include "renderer.hpp"

namespace renderer {

    Renderer::Renderer(RendererConfiguration &configuration) {

        std::cout << "created renderer" << std::endl;

        VulkanConfiguration vulkanConfiguration{
                .window = configuration.window,

                .engineName = ENGINE_NAME,
                .applicationName = configuration.applicationName,
                .engineVersion = ENGINE_VERSION,
                .applicationVersion = configuration.applicationVersion,

                .debug = configuration.debug,

                .preferredSurfaceFormats = {},
                .requiredInstanceExtensions = {},
                .requiredInstanceLayers = {},
                .requiredDeviceExtensions = {}
        };

        initializeEngine(vulkanConfiguration);
    }

    Renderer::~Renderer() {
        destroyEngine();

        std::cout << "destroyed renderer" << std::endl;
    }
}