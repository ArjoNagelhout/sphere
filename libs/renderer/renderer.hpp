#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "engine.hpp"
#include "swapchain.hpp"
#include "render_pass.hpp"
#include "graphics_pipeline.hpp"

namespace renderer {

    struct RendererConfiguration {
        GLFWwindow *window;

        bool debug;

        const std::string applicationName;
        const uint32_t applicationVersion;
    };

    class Renderer {

    public:
        explicit Renderer(RendererConfiguration &configuration);
        ~Renderer();

    private:
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    };
}

#endif //SPHERE_RENDERER_HPP
