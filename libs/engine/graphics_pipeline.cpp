#include "engine.h"
#include "graphics_pipeline.h"

#include <fstream>

namespace engine {
    GraphicsPipeline::GraphicsPipeline(Swapchain &swapchain, RenderPass &renderPass) : swapchain(swapchain),
                                                                                       renderPass(renderPass) {
        createGraphicsPipeline();
    }

    GraphicsPipeline::~GraphicsPipeline() {
        destroyGraphicsPipeline();
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

    VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char> &code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shaderModule;
        checkResult(vkCreateShaderModule(engine->device, &createInfo, nullptr, &shaderModule));

        std::cout << "created shader module" << std::endl;
        return shaderModule;
    }

    /*
     * Graphics pipeline consists of a lot of steps, e.g. vertex input, rasterization
     *
     * Some are fixed and can be set on pipeline creation, and others are done via shaders etc.
     */
    void GraphicsPipeline::createGraphicsPipeline() {

        std::vector<char> vertexShaderCode = readFile("shaders/shader_vert.spv");
        std::vector<char> fragmentShaderCode = readFile("shaders/shader_frag.spv");

        VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
        VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

        VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
        vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageInfo.module = vertexShaderModule;
        vertexShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageInfo.module = fragmentShaderModule;
        fragmentShaderStageInfo.pName = "main";

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos = {
                vertexShaderStageInfo,
                fragmentShaderStageInfo
        };

        VkVertexInputBindingDescription vertexInputBindingDescription{
                .binding = 0, // binding number (identifier?)
                .stride = sizeof(VertexAttributes), //physicalDeviceData.minVertexInputBindingStrideAlignment, // the amount of bytes per item in the vertex buffer
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        //must contain VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT,
        VkVertexInputAttributeDescription pos{
                .location = 0, // is the shader input location number for this attribute
                .binding = 0, // binding number
                .format = VK_FORMAT_R32G32B32_SFLOAT, // super confusing, but vec3 translates to this format
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
        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexBindingDescriptions = &vertexInputBindingDescription;
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
        vertexInputState.pVertexAttributeDescriptions = attributes.data();

        // how do vertices get converted into a primitive (i.e. triangle)
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyState.primitiveRestartEnable = VK_FALSE;

        // VkPipelineTessellationStateCreateInfo tesselationState{};
        // tesselationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        // tesselationState.patchControlPoints

        VkExtent2D extent = swapchain.extent;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) extent.width;
        viewport.height = (float) extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizationState{};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.depthClampEnable = VK_FALSE;
        rasterizationState.rasterizerDiscardEnable = VK_FALSE;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;// VK_POLYGON_MODE_LINE for wireframe
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationState.depthBiasEnable = VK_FALSE;
        rasterizationState.depthBiasConstantFactor = 0.0f;
        rasterizationState.depthBiasClamp = 0.0f;
        rasterizationState.depthBiasSlopeFactor = 0.0f;
        rasterizationState.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisampleState{};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleState.sampleShadingEnable = VK_FALSE;
        multisampleState.minSampleShading = 1.0f;
        multisampleState.pSampleMask = nullptr;
        multisampleState.alphaToCoverageEnable = VK_FALSE;
        multisampleState.alphaToOneEnable = VK_FALSE;

//        VkStencilOpState frontState{
//            .failOp
//        };

        VkPipelineDepthStencilStateCreateInfo depthStencilState{};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.depthTestEnable = VK_TRUE;
        depthStencilState.depthWriteEnable = VK_TRUE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE;
        depthStencilState.front = {};
        depthStencilState.back = {};
        depthStencilState.minDepthBounds = 0.0f;
        depthStencilState.maxDepthBounds = 1.0f;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendState{};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.logicOpEnable = VK_FALSE;
        colorBlendState.logicOp = VK_LOGIC_OP_COPY;
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &colorBlendAttachment;
        colorBlendState.blendConstants[0] = 0.0f;
        colorBlendState.blendConstants[1] = 0.0f;
        colorBlendState.blendConstants[2] = 0.0f;
        colorBlendState.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
        };

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = 1;
        descriptorSetLayoutInfo.pBindings = &descriptorSetLayoutBinding;

        checkResult(
                vkCreateDescriptorSetLayout(engine->device, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout));

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        checkResult(vkCreatePipelineLayout(engine->device, &pipelineLayoutCreateInfo, nullptr, &graphicsPipelineLayout));

        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
        createInfo.pStages = shaderStageInfos.data();
        createInfo.pVertexInputState = &vertexInputState;
        createInfo.pInputAssemblyState = &inputAssemblyState;
        createInfo.pTessellationState = nullptr;
        createInfo.pViewportState = &viewportState;
        createInfo.pRasterizationState = &rasterizationState;
        createInfo.pMultisampleState = &multisampleState;
        createInfo.pDepthStencilState = &depthStencilState;
        createInfo.pColorBlendState = &colorBlendState;
        createInfo.pDynamicState = &dynamicState;
        createInfo.layout = graphicsPipelineLayout;
        createInfo.renderPass = renderPass.renderPass;
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE; // / optional, can be used to create a new graphics pipeline by deriving from an existing.pipeline, makes switching quicker.
        createInfo.basePipelineIndex = -1;

        checkResult(vkCreateGraphicsPipelines(engine->device, VK_NULL_HANDLE, 1, &createInfo, nullptr,
                                              &graphicsPipeline));
        std::cout << "created graphics pipeline" << std::endl;

        vkDestroyShaderModule(engine->device, vertexShaderModule, nullptr);
        vkDestroyShaderModule(engine->device, fragmentShaderModule, nullptr);
    }

    void GraphicsPipeline::destroyGraphicsPipeline() {
        vkDestroyDescriptorSetLayout(engine->device, descriptorSetLayout, nullptr);
        vkDestroyPipeline(engine->device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(engine->device, graphicsPipelineLayout, nullptr);
    }
}