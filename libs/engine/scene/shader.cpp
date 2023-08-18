#include "engine.h"
#include "shader.h"

namespace engine {

    Shader::Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
                engine->descriptorSetBuilder->descriptorSetLayout
        };

        PipelineData &data = engine->pipelineBuilder->createPipeline(engine->renderPass->renderPass, descriptorSetLayouts, vertexShaderPath, fragmentShaderPath);
        pipelineData = &data; // get pointer to pipeline data (unowned pointer)
    }

    Shader::~Shader() = default;
}