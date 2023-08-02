#ifndef SPHERE_RENDER_PASS_HPP
#define SPHERE_RENDER_PASS_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <device.hpp>
#include <swapchain.hpp>

namespace renderer {

    class RenderPass {

    public:
        explicit RenderPass(Device &device, Swapchain &swapchain) : device(device), swapchain(swapchain) {
            createRenderPass(device.getDevice(), swapchain.getSwapchainImageFormat(), renderPass);
        }

        ~RenderPass() {
            vkDestroyRenderPass(device.getDevice(), renderPass, nullptr);
        }

        const VkRenderPass &getRenderPass() {
            return renderPass;
        }

    private:
        Device &device;
        Swapchain &swapchain;
        VkRenderPass renderPass;

        /*
         * A render pass defines a set of image resources (attachments) to be used during rendering.
         *
         * draw commands (vkCmdDraw...) need to be recorded into a render pass instance using vkCmdBeginRenderPass
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

    };

}

#endif //SPHERE_RENDER_PASS_HPP
