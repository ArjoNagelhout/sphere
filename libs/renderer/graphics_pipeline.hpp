#ifndef SPHERE_GRAPHICS_PIPELINE_HPP
#define SPHERE_GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <device.hpp>
#include <swapchain.hpp>

#include <stdexcept>

namespace renderer {

    /*
     * Pipelines should be cached in a pipeline cache.
     *
     * There can be many pipelines, so this should be refactored to support different shaders and materials etc.
     */
    class GraphicsPipeline {

    public:
        explicit GraphicsPipeline(Device &device, Swapchain &swapchain) : device(device), swapchain(swapchain) {
            createRenderPass(device.getDevice(), swapchain.getSwapchainImageFormat(), renderPass);
            createGraphicsPipeline(device.getDevice(), swapchain, renderPass, graphicsPipeline);
        }

        ~GraphicsPipeline() {
            vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
        }

    private:
        Device &device;
        Swapchain &swapchain;
        VkRenderPass renderPass;
        VkPipeline graphicsPipeline;

        /*
         * A render pass defines a set of image resources (attachments) to be used during rendering.
         *
         * draw commands (vkCmdDraw...) need to be recorded into a render pass instance.
         *
         * VkRenderPass represents collection of attachments, subpasses, dependencies between these subpasses, and
         * describes how attachments are handled between these subpasses.
         */
        static void createRenderPass(const VkDevice &device, const VkFormat &format, VkRenderPass &renderPass) {

            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = format;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // for multisampling
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentReference{};
            colorAttachmentReference.attachment = 0; // index corresponding to the render pass create info pAttachments
            colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.inputAttachmentCount = 0; // optional
            subpass.pInputAttachments = nullptr; // optional
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentReference;
            subpass.pResolveAttachments = nullptr; // optional
            subpass.pDepthStencilAttachment = nullptr; // optional
            subpass.preserveAttachmentCount = 0; // optional
            subpass.pPreserveAttachments = nullptr; // optional

            // subpass dependency is "glue" between different subpasses
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependency.dstSubpass = 0;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.srcAccessMask = 0;
            subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            //subpassDependency.dependencyFlags

            VkRenderPassCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &colorAttachment;
            createInfo.subpassCount = 1;
            createInfo.pSubpasses = &subpass;
            createInfo.dependencyCount = 1;
            createInfo.pDependencies = &subpassDependency;

            VkResult result = vkCreateRenderPass(device, &createInfo, nullptr, &renderPass);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create render pass: ") + string_VkResult(result));
            }

            std::cout << "created render pass" << std::endl;
        }

        /*
         * Graphics pipeline consists of a lot of steps, e.g. vertex input, rasterization
         *
         * Some are fixed and can be set on pipeline creation, and others are done via shaders etc.
         */
        static void createGraphicsPipeline(const VkDevice &device,
                                           Swapchain &swapchain,
                                           const VkRenderPass &renderPass,
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

            // how are vertices input into the pipeline
            VkPipelineVertexInputStateCreateInfo vertexInputState{};
            vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputState.vertexBindingDescriptionCount = 0;
            vertexInputState.pVertexBindingDescriptions = nullptr; // array of VkVertexInputBindingDescription
            vertexInputState.vertexAttributeDescriptionCount = 0;
            vertexInputState.pVertexAttributeDescriptions = nullptr;

            // how do vertices get converted into a primitive (i.e. triangle)
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
            inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssemblyState.primitiveRestartEnable = VK_FALSE;

            // VkPipelineTessellationStateCreateInfo tesselationState{};
            // tesselationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
            // tesselationState.patchControlPoints =

            VkExtent2D extent = swapchain.getSwapchainExtent();

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            //viewport.width = swapchain.getSwapchainExtent();
            // viewport.height
            //viewport.minDepth
            //viewport.maxDepth

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            //viewportState.viewportCount
            //viewportState.pViewports
            //viewportState.scissorCount
            //viewportState.pScissors

            VkPipelineRasterizationStateCreateInfo rasterizationState{};
            rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            //rasterizationState.depthClampEnable
            //rasterizationState.rasterizerDiscardEnable
            //rasterizationState.polygonMode
            //rasterizationState.cullMode
            //rasterizationState.frontFace
            //rasterizationState.depthBiasEnable
            //rasterizationState.depthBiasConstantFactor
            //rasterizationState.depthBiasClamp
            //rasterizationState.depthBiasSlopeFactor
            //rasterizationState.lineWidth

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
            createInfo.pTessellationState = nullptr;
            createInfo.pViewportState = &viewportState;
            createInfo.pRasterizationState = &rasterizationState;
            createInfo.pMultisampleState = &multisampleState;
            createInfo.pDepthStencilState = &depthStencilState;
            createInfo.pColorBlendState = &colorBlendState;
            createInfo.pDynamicState = &dynamicState;
            createInfo.layout = pipelineLayout;
            createInfo.renderPass = renderPass;
            createInfo.subpass = 0;
            createInfo.basePipelineHandle = VK_NULL_HANDLE; // / optional, can be used to create a new graphics pipeline by deriving from an existing.pipeline, makes switching quicker.
            createInfo.basePipelineIndex = -1;

            VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create graphics pipeline: ") + string_VkResult(result));
            }

            std::cout << "created graphics pipeline" << std::endl;
        }
    };
}

#endif //SPHERE_GRAPHICS_PIPELINE_HPP
