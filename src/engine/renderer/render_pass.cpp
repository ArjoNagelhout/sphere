#include "vulkan_context.h"
#include "render_pass.h"

namespace engine::renderer {

    RenderPass::RenderPass(const VkFormat &format, const VkFormat &depthFormat) {
        VkAttachmentDescription colorAttachment{
                .format = format,
                .samples = VK_SAMPLE_COUNT_1_BIT, // for multisampling
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference colorAttachmentReference{
                .attachment = 0, // index corresponding to the render pass create info pAttachments
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkAttachmentDescription depthAttachment{
                .format = depthFormat, // D is for depth, S is for stencil, e.g. VK_FORMAT_D16_UNORM_S8_UINT
                .samples = VK_SAMPLE_COUNT_1_BIT, // for multisampling
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        VkAttachmentReference depthAttachmentReference{
                .attachment = 1, // index corresponding to the render pass create info pAttachments
                .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpass{
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .inputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .colorAttachmentCount = 1,
                .pColorAttachments = &colorAttachmentReference,
                .pResolveAttachments = nullptr,
                .pDepthStencilAttachment = &depthAttachmentReference,
                .preserveAttachmentCount = 0,
                .pPreserveAttachments = nullptr,
        };

        // subpass dependency is "glue" between different subpasses
        VkSubpassDependency subpassDependency{
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,//|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        };

        // subpass dependency is "glue" between different subpasses
        VkSubpassDependency depthSubpassDependency{
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,//|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        };

        std::vector<VkSubpassDependency> dependencies{
                subpassDependency,
                depthSubpassDependency
        };

        std::vector<VkAttachmentDescription> attachments{
                colorAttachment,
                depthAttachment
        };

        VkRenderPassCreateInfo createInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments = attachments.data(),
                .subpassCount = 1,
                .pSubpasses = &subpass,
                .dependencyCount = static_cast<uint32_t>(dependencies.size()),
                .pDependencies = dependencies.data(),
        };

        checkResult(vkCreateRenderPass(context->device, &createInfo, nullptr, &renderPass));
        std::cout << "created render pass" << std::endl;
    }

    RenderPass::~RenderPass() {
        vkDestroyRenderPass(context->device, renderPass, nullptr);
    }
}