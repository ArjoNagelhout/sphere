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

        swapchain = std::make_unique<Swapchain>(preferredSurfaceFormats);
        renderPass = std::make_unique<RenderPass>(swapchain->surfaceFormat.format);
        graphicsPipeline = std::make_unique<GraphicsPipeline>(*swapchain, *renderPass);
    }

    Renderer::~Renderer() {
        graphicsPipeline.reset();
        renderPass.reset();
        swapchain.reset();

        destroyEngine();

        std::cout << "destroyed renderer" << std::endl;
    }
}