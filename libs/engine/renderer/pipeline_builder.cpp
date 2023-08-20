#include "engine.h"
#include "pipeline_builder.h"

#include <fstream>

namespace engine {

    PipelineData::PipelineData(const VkPipeline &pipeline, const VkPipelineLayout &pipelineLayout) : pipeline(pipeline),
                                                                                                     pipelineLayout(
                                                                                                             pipelineLayout) {

    }

    PipelineBuilder::PipelineBuilder() {
//
//        VkPipelineCacheCreateInfo pipelineCacheInfo{};
//        pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
//        pipelineCacheInfo.pNext = nullptr;
//        //pipelineCacheInfo.initialDataSize
//        //pipelineCacheInfo.pInitialData
    }

    PipelineBuilder::~PipelineBuilder() {
        for (const auto &pipeline: pipelines) {
            vkDestroyPipeline(engine->device, pipeline->pipeline, nullptr);
            vkDestroyPipelineLayout(engine->device, pipeline->pipelineLayout, nullptr);
        }
    }

    static std::vector<char> readFile(const std::string &filename) {
        std::ifstream file(filename,
                           std::ios::ate |
                           // start reading at the end of the file, because we can use the read position to know how big of a buffer we need to allocate
                           std::ios::binary); // read as binary file, no text transformations

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file");
        }

        size_t fileSize = (size_t) file.tellg();

        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    VkShaderModule PipelineBuilder::createShaderModule(const std::vector<char> &code) {
        VkShaderModuleCreateInfo createInfo{
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = code.size(),
                .pCode = reinterpret_cast<const uint32_t *>(code.data()),
        };

        VkShaderModule shaderModule;
        checkResult(vkCreateShaderModule(engine->device, &createInfo, nullptr, &shaderModule));

        std::cout << "created shader module" << std::endl;
        return shaderModule;
    }

    PipelineData &PipelineBuilder::createPipeline(const VkRenderPass &renderPass,
                                                  const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                                                  const std::string &vertexShaderPath,
                                                  const std::string &fragmentShaderPath) {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        std::string shadersDirectory = "shaders/";

        std::vector<char> vertexShaderCode = readFile(shadersDirectory + vertexShaderPath);
        std::vector<char> fragmentShaderCode = readFile(shadersDirectory + fragmentShaderPath);

        VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
        VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

        VkPipelineShaderStageCreateInfo vertexShaderStageInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShaderModule,
                .pName = "main",
        };

        VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShaderModule,
                .pName = "main",
        };

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos{
                vertexShaderStageInfo,
                fragmentShaderStageInfo
        };

        VkVertexInputBindingDescription vertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(VertexAttributes),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        VkVertexInputAttributeDescription pos{
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = 0
        };

        VkVertexInputAttributeDescription uv{
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT, // vec2
                .offset = sizeof(VertexAttributes::position)
        };

        VkVertexInputAttributeDescription normal{
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT, // vec3
                .offset = sizeof(VertexAttributes::position) + sizeof(VertexAttributes::uv)
        };

        std::vector<VkVertexInputAttributeDescription> attributes{
                pos,
                uv,
                normal
        };

        // how are vertices input into the pipeline
        VkPipelineVertexInputStateCreateInfo vertexInputState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &vertexInputBindingDescription,
                .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
                .pVertexAttributeDescriptions = attributes.data(),
        };

        // how do vertices get converted into a primitive (i.e. triangle)
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE,
        };

        // VkPipelineTessellationStateCreateInfo tesselationState{};
        // tesselationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        // tesselationState.patchControlPoints

        VkExtent2D extent = engine->swapchain->extent;

        VkViewport viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = (float) extent.width,
                .height = (float) extent.height,
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
        };

        VkRect2D scissor{
                .offset = {0, 0},
                .extent = extent,
        };

        VkPipelineViewportStateCreateInfo viewportState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = 1,
                .pViewports = &viewport,
                .scissorCount = 1,
                .pScissors = &scissor,
        };

        VkPipelineRasterizationStateCreateInfo rasterizationState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = VK_POLYGON_MODE_FILL,// VK_POLYGON_MODE_LINE for wireframe
                .cullMode = VK_CULL_MODE_BACK_BIT,
                .frontFace = VK_FRONT_FACE_CLOCKWISE,
                .depthBiasEnable = VK_FALSE,
                .depthBiasConstantFactor = 0.0f,
                .depthBiasClamp = 0.0f,
                .depthBiasSlopeFactor = 0.0f,
                .lineWidth = 1.0f,
        };

        VkPipelineMultisampleStateCreateInfo multisampleState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .sampleShadingEnable = VK_FALSE,
                .minSampleShading = 1.0f,
                .pSampleMask = nullptr,
                .alphaToCoverageEnable = VK_FALSE,
                .alphaToOneEnable = VK_FALSE,
        };

        VkPipelineDepthStencilStateCreateInfo depthStencilState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
                .front = {},
                .back = {},
                .minDepthBounds = 0.0f,
                .maxDepthBounds = 1.0f,
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment{
                .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                  VK_COLOR_COMPONENT_A_BIT,
        };

        VkPipelineColorBlendStateCreateInfo colorBlendState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable = VK_FALSE,
                .logicOp = VK_LOGIC_OP_COPY,
                .attachmentCount = 1,
                .pAttachments = &colorBlendAttachment,
                .blendConstants {0.0f, 0.0f, 0.0f, 0.0f}
        };

        std::vector<VkDynamicState> dynamicStates{
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
                .pDynamicStates = dynamicStates.data(),
        };

        VkPushConstantRange pushConstantRange{
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
        };

        std::vector<VkPushConstantRange> pushConstantRanges{
                pushConstantRange
        };

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
                .pSetLayouts = descriptorSetLayouts.data(),
                .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
                .pPushConstantRanges = pushConstantRanges.data(),
        };

        checkResult(
                vkCreatePipelineLayout(engine->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

        VkGraphicsPipelineCreateInfo createInfo{
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .stageCount = static_cast<uint32_t>(shaderStageInfos.size()),
                .pStages = shaderStageInfos.data(),
                .pVertexInputState = &vertexInputState,
                .pInputAssemblyState = &inputAssemblyState,
                .pTessellationState = nullptr,
                .pViewportState = &viewportState,
                .pRasterizationState = &rasterizationState,
                .pMultisampleState = &multisampleState,
                .pDepthStencilState = &depthStencilState,
                .pColorBlendState = &colorBlendState,
                .pDynamicState = &dynamicState,
                .layout = pipelineLayout,
                .renderPass = renderPass,
                .subpass = 0,
                .basePipelineHandle = VK_NULL_HANDLE, // / optional, can be used to create a new graphics pipeline by deriving from an existing.pipeline, makes switching quicker
                .basePipelineIndex = -1
        };

        checkResult(vkCreateGraphicsPipelines(engine->device, VK_NULL_HANDLE, 1, &createInfo, nullptr,
                                              &pipeline));

        std::cout << "created pipeline" << std::endl;

        vkDestroyShaderModule(engine->device, vertexShaderModule, nullptr);
        vkDestroyShaderModule(engine->device, fragmentShaderModule, nullptr);

        pipelines.emplace_back(std::make_unique<PipelineData>(pipeline, pipelineLayout));
        return *pipelines.back();
    }
}