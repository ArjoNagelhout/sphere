#include "engine.h"
#include "shader.h"

namespace engine {

    Shader::Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {

        descriptorSetLayout = createDescriptorSetLayout();

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
                descriptorSetLayout
        };

        PipelineData &data = engine->pipelineBuilder->createPipeline(engine->renderPass->renderPass, descriptorSetLayouts, vertexShaderPath, fragmentShaderPath);
        pipelineData = &data; // get pointer to pipeline data (unowned pointer)
    }

    Shader::~Shader() {
        vkDestroyDescriptorSetLayout(engine->device, descriptorSetLayout, nullptr);
    }
}