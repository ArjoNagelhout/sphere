#include "vulkan_context.h"
#include "shader.h"
#include "descriptor_set_builder.h"

namespace engine::renderer {

    Shader::Shader(const std::string &vertexShaderPath,
                   const std::string &fragmentShaderPath,
                   VkRenderPass renderPass) : renderPass(renderPass) {

        descriptorSetLayout = createDescriptorSetLayout();

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
                descriptorSetLayout
        };

        PipelineData &data = pipelineBuilder->createPipeline(renderPass, descriptorSetLayouts, vertexShaderPath,
                                                            fragmentShaderPath);
        pipelineData = &data; // get pointer to pipeline data (unowned pointer)
    }

    Shader::~Shader() {
        vkDestroyDescriptorSetLayout(context->device, descriptorSetLayout, nullptr);
    }
}