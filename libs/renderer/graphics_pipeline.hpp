#ifndef SPHERE_GRAPHICS_PIPELINE_HPP
#define SPHERE_GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <device.hpp>
#include <swapchain.hpp>
#include <render_pass.hpp>

#include <fstream>
#include <stdexcept>

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
        explicit GraphicsPipeline(Device &device, Swapchain &swapchain, RenderPass &renderPass) : device(device), swapchain(swapchain), renderPass(renderPass) {
            createGraphicsPipeline(device, swapchain, renderPass.getRenderPass(), graphicsPipeline, graphicsPipelineLayout, descriptorSetLayout, descriptorType);
        }

        ~GraphicsPipeline() {
            vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(device.getDevice(), graphicsPipelineLayout, nullptr);
        }

        const VkPipeline &getGraphicsPipeline() {
            return graphicsPipeline;
        }

        const VkPipelineLayout &getGraphicsPipelineLayout() {
            return graphicsPipelineLayout;
        }

        const VkDescriptorSetLayout &getDescriptorSetLayout() {
            return descriptorSetLayout;
        }

        const VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    private:
        Device &device;
        Swapchain &swapchain;
        RenderPass &renderPass;

        VkPipeline graphicsPipeline;
        VkPipelineLayout graphicsPipelineLayout;
        VkDescriptorSetLayout descriptorSetLayout;

        /*
         * Graphics pipeline consists of a lot of steps, e.g. vertex input, rasterization
         *
         * Some are fixed and can be set on pipeline creation, and others are done via shaders etc.
         */
        static void createGraphicsPipeline(Device &device,
                                           Swapchain &swapchain,
                                           const VkRenderPass &renderPass,
                                           VkPipeline &graphicsPipeline,
                                           VkPipelineLayout &graphicsPipelineLayout,
                                           VkDescriptorSetLayout &descriptorSetLayout,
                                           const VkDescriptorType &descriptorType) {

            std::vector<char> vertexShaderCode = readFile("shaders/shader_vert.spv");
            std::vector<char> fragmentShaderCode = readFile("shaders/shader_frag.spv");

            VkShaderModule vertexShaderModule = createShaderModule(device.getDevice(), vertexShaderCode);
            VkShaderModule fragmentShaderModule = createShaderModule(device.getDevice(), fragmentShaderCode);

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

            PhysicalDeviceData physicalDeviceData = device.getPhysicalDeviceData();

            VkVertexInputBindingDescription vertexInputBindingDescription{
                .binding = 0, // binding number (identifier?)
                .stride = physicalDeviceData.minVertexInputBindingStrideAlignment, // the amount of bytes per item in the vertex buffer
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            };

//            VkVertexInputAttributeDescription vertexInputAttributeDescription{
//                .location = 0, // is the shader input location number for this attribute
//                .binding = 0, // binding number
//                // .format = must contain VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT,
//                .offset = 0
//            };

            // how are vertices input into the pipeline
            VkPipelineVertexInputStateCreateInfo vertexInputState{};
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputState.vertexBindingDescriptionCount = 1;
            vertexInputState.pVertexBindingDescriptions = &vertexInputBindingDescription;
            vertexInputState.vertexAttributeDescriptionCount = 0; // 1;
            vertexInputState.pVertexAttributeDescriptions = nullptr; //&vertexInputAttributeDescription;

            // how do vertices get converted into a primitive (i.e. triangle)
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
            inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssemblyState.primitiveRestartEnable = VK_FALSE;

            // VkPipelineTessellationStateCreateInfo tesselationState{};
            // tesselationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
            // tesselationState.patchControlPoints

            VkExtent2D extent = swapchain.getSwapchainExtent();

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)extent.width;
            viewport.height = (float)extent.height;
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
            rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
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

            //VkPipelineDepthStencilStateCreateInfo depthStencilState{};
            //depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            //depthStencilState.depthTestEnable
            //depthStencilState.depthWriteEnable
            //depthStencilState.depthCompareOp
            //depthStencilState.depthBoundsTestEnable
            //depthStencilState.stencilTestEnable
            //depthStencilState.front
            //depthStencilState.back
            //depthStencilState.minDepthBounds
            //depthStencilState.maxDepthBounds

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
                .descriptorType = descriptorType,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            };

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
            descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutInfo.bindingCount = 1;
            descriptorSetLayoutInfo.pBindings = &descriptorSetLayoutBinding;

            VkResult descriptorSetLayoutResult = vkCreateDescriptorSetLayout(device.getDevice(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout);

            if (descriptorSetLayoutResult != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create descriptor set layout: ") + string_VkResult(descriptorSetLayoutResult));
            }

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.setLayoutCount = 1;
            pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
            pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

            VkResult pipelineLayoutResult = vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutCreateInfo, nullptr, &graphicsPipelineLayout);

            if (pipelineLayoutResult != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create pipeline layout: ") + string_VkResult(pipelineLayoutResult));
            }

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
            createInfo.pDepthStencilState = nullptr; //&depthStencilState;
            createInfo.pColorBlendState = &colorBlendState;
            createInfo.pDynamicState = &dynamicState;
            createInfo.layout = graphicsPipelineLayout;
            createInfo.renderPass = renderPass;
            createInfo.subpass = 0;
            createInfo.basePipelineHandle = VK_NULL_HANDLE; // / optional, can be used to create a new graphics pipeline by deriving from an existing.pipeline, makes switching quicker.
            createInfo.basePipelineIndex = -1;

            VkResult result = vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create graphics pipeline: ") + string_VkResult(result));
            }

            vkDestroyShaderModule(device.getDevice(), vertexShaderModule, nullptr);
            vkDestroyShaderModule(device.getDevice(), fragmentShaderModule, nullptr);

            std::cout << "created graphics pipeline" << std::endl;
        }

        static std::vector<char> readFile(const std::string& filename) {
            std::ifstream file(filename,
                               std::ios::ate | // start reading at the end of the file, because we can use the read position to know how big of a buffer we need to allocate
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

        static VkShaderModule createShaderModule(const VkDevice &device, const std::vector<char> &code) {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create shader module: ") + string_VkResult(result));
            }

            std::cout << "created shader module" << std::endl;

            return shaderModule;
        }
    };
}

#endif //SPHERE_GRAPHICS_PIPELINE_HPP
