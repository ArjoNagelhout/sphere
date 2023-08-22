#ifndef SPHERE_MATERIAL_SYSTEM_H
#define SPHERE_MATERIAL_SYSTEM_H

#include "texture.h"

#include "vulkan.h"
#include "swapchain.h"

namespace engine::renderer {

    struct PipelineData {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        explicit PipelineData(const VkPipeline &pipeline, const VkPipelineLayout &pipelineLayout);
    };

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
     * note: graphics pipeline is completely immutable, except for the dynamic states
     */
    class PipelineBuilder {

    public:
        explicit PipelineBuilder(Swapchain &swapchain);
        ~PipelineBuilder();

        // should be destroyed
        std::vector<std::unique_ptr<PipelineData>> pipelines;

        PipelineData &createPipeline(const VkRenderPass &renderPass, const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                                     const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    private:
        Swapchain &swapchain;
        VkPipelineCache pipelineCache;

        static VkShaderModule createShaderModule(const std::vector<char> &code);
    };

    extern PipelineBuilder *pipelineBuilder;

    /*
     * A shader is a template from which materials can be built.
     * A shader defines all possible properties and allows the DescriptorSetBuilder and the PipelineBuilder to
     * construct the proper representations in Vulkan objects.
     *
     * Properties can be: matrices, textures, floats, vectors
     *
     * These properties get set using descriptor sets
     */
    class Shader {

    public:
        explicit Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath, VkRenderPass renderPass);

        ~Shader();

        PipelineData *pipelineData; // (unowned pointer)
        VkDescriptorSetLayout descriptorSetLayout;

    private:
        VkRenderPass renderPass;
    };
    /*
     * A material contains a reference to a shader and contains the properties such as
     * a texture.
     */
    class Material {

    public:
        explicit Material(const Shader &shader, Texture &texture);
        ~Material();

        const Shader &shader;
        renderer::Texture &texture;

        VkDescriptorSet descriptorSet;

    private:

    };


}

#endif //SPHERE_MATERIAL_SYSTEM_H
