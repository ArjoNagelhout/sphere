#ifndef SPHERE_GRAPHICS_PIPELINE_HPP
#define SPHERE_GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <device.hpp>

#include <stdexcept>

namespace renderer {

    /*
     * Pipelines should be cached in a pipeline cache.
     *
     * There can be many pipelines, so this should be refactored to support different shaders and materials etc.
     */
    class GraphicsPipeline {

    public:
        explicit GraphicsPipeline(Device device) {
            createGraphicsPipeline(device.getDevice());
        }

        ~GraphicsPipeline() {
            //vkDestroyPipeline()
        }

    private:

        static void createRenderPass(const VkDevice &device, const VkFormat &format, VkRenderPass &renderPass) {

            VkAttachmentDescription attachment{};
            attachment.format = format;
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkSubpassDescription subpass{};

            VkRenderPassCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            //createInfo.flags
            //createInfo.attachmentCount
            //createInfo.pAttachments
            //createInfo.subpassCount
            //createInfo.pSubpasses
            //createInfo.dependencyCount
            //createInfo.pDependencies

            VkResult result = vkCreateRenderPass(device, &createInfo, nullptr, &renderPass);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create render pass: ") + string_VkResult(result));
            }

            std::cout << "created render pass" << std::endl;
        }

        static void createGraphicsPipeline(const VkDevice &device,
                                           VkPipeline &graphicsPipeline) {

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.setLayoutCount = 0;
            pipelineLayoutCreateInfo.pSetLayouts = nullptr;
            pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

            VkPipelineLayout pipelineLayout;
            VkResult pipelineLayoutResult = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

            if (pipelineLayoutResult != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create pipeline layout: ") + string_VkResult(pipelineLayoutResult));
            }

            VkPipelineVertexInputStateCreateInfo vertexInputState{};
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

            VkPipelineTessellationStateCreateInfo tesselationState{};
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

            VkPipelineViewportStateCreateInfo viewportState{};
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

            VkPipelineRasterizationStateCreateInfo rasterizationState{};
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

            VkPipelineMultisampleStateCreateInfo multisampleState{};
            multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

            VkPipelineDepthStencilStateCreateInfo depthStencilState{};
            depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

            VkPipelineColorBlendStateCreateInfo colorBlendState{};
            colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

            VkGraphicsPipelineCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            //createInfo.flags
            //createInfo.stageCount
            //createInfo.pStages
            createInfo.pVertexInputState = &vertexInputState;
            createInfo.pInputAssemblyState = &inputAssemblyState;
            createInfo.pTessellationState = &tesselationState;
            createInfo.pViewportState = &viewportState;
            createInfo.pRasterizationState = &rasterizationState;
            createInfo.pMultisampleState = &multisampleState;
            createInfo.pDepthStencilState = &depthStencilState;
            createInfo.pColorBlendState = &colorBlendState;
            createInfo.pDynamicState = &dynamicState;
            createInfo.layout = pipelineLayout;
            //createInfo.renderPass
            //createInfo.subpass
            //createInfo.basePipelineHandle
            //createInfo.basePipelineIndex

            VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create graphics pipeline: ") + string_VkResult(result));
            }

            std::cout << "created graphics pipeline" << std::endl;
        }
    };
}

#endif //SPHERE_GRAPHICS_PIPELINE_HPP
