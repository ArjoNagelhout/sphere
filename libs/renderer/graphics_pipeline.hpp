#ifndef SPHERE_GRAPHICS_PIPELINE_HPP
#define SPHERE_GRAPHICS_PIPELINE_HPP

#include "engine.hpp"
#include "swapchain.hpp"
#include "render_pass.hpp"
#include "types.hpp"

namespace renderer {

    /*
     * Pipelines should be cached in a pipeline cache.
     *
     * There can be many pipelines, so this should be refactored to support different shaders and materials etc.
     *
     * Order of execution of a graphics pipeline:
     *
     * INPUT: vertex / index buffer
     * 1. FIXED input assembler (collects raw vertex data from (index) buffers)
     * 2. PROGRAMMABLE vertex shader (applies transformations, e.g. from model space to screen space)
     * 3. PROGRAMMABLE tessellation (subdivide geometry based on rules to increase mesh quality)
     * 4. PROGRAMMABLE geometry shader (not used, inefficient)
     * 5. FIXED rasterization (transforms primitives into fragments (pixels), discard fragments using depth testing)
     * 6. PROGRAMMABLE fragment shader (does things per pixel)
     * 7. FIXED color blending (applies operations for overlapping fragments (same pixel coordinates))
     * OUTPUT: framebuffer
     *
     * note: graphics pipeline is completely immutable
     */
    class GraphicsPipeline {

    public:
        explicit GraphicsPipeline(Swapchain &swapchain, RenderPass &renderPass);
        ~GraphicsPipeline();

    private:
        Engine &engine;
        Swapchain &swapchain;
        RenderPass &renderPass;

        VkPipeline graphicsPipeline;
        VkPipelineLayout graphicsPipelineLayout;
        VkDescriptorSetLayout descriptorSetLayout;

        void createGraphicsPipeline();
        void destroyGraphicsPipeline();
        VkShaderModule createShaderModule(const std::vector<char> &code);
    };

}

#endif //SPHERE_GRAPHICS_PIPELINE_HPP
