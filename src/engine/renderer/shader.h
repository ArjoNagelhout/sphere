#ifndef SPHERE_SHADER_H
#define SPHERE_SHADER_H

#include <string>
#include "pipeline_builder.h"

namespace engine::renderer {

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
}

#endif //SPHERE_SHADER_H
