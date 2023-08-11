#include "engine.h"
#include "render_pass.h"

namespace engine {

    RenderPass::RenderPass(const VkFormat &format) {
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

//        VkPhysicalDeviceFeatures features;
//        vkGetPhysicalDeviceFeatures(context.physicalDevice, &features);

        // check if depth format is supported
//        std::vector<VkFormat> depthFormats{
//            VK_FORMAT_R8_UNORM,
//            VK_FORMAT_R16_UNORM,
//            VK_FORMAT_D16_UNORM,
//            VK_FORMAT_D16_UNORM_S8_UINT,
//            VK_FORMAT_D24_UNORM_S8_UINT,
//            VK_FORMAT_D32_SFLOAT,
//            VK_FORMAT_D32_SFLOAT_S8_UINT,
//            VK_FORMAT_S8_UINT
//        };
//
//        for (auto const &depthFormat : depthFormats) {
//            VkFormatProperties properties;
//            vkGetPhysicalDeviceFormatProperties(engine->physicalDevice, depthFormat, &properties);
//
//            std::cout << "\nformat: " << string_VkFormat(depthFormat)
//                << "\nbufferFeatures: " + string_VkFormatFeatureFlags(properties.bufferFeatures)
//                << "\nlinearTilingFeatures: " << string_VkFormatFeatureFlags(properties.linearTilingFeatures)
//                << "\noptimalTilingFeatures: " << string_VkFormatFeatureFlags(properties.optimalTilingFeatures)
//                << std::endl;
//        }

//        VkAttachmentDescription depthAttachment{};
//        depthAttachment.format = VK_FORMAT_D16_UNORM; // D is for depth, S is for stencil, e.g. VK_FORMAT_D16_UNORM_S8_UINT
//        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // for multisampling
//        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
//
//        VkAttachmentReference depthAttachmentReference{};
//        depthAttachmentReference.attachment = 1; // index corresponding to the render pass create info pAttachments
//        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0; // optional
        subpass.pInputAttachments = nullptr; // optional
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;
        subpass.pResolveAttachments = nullptr; // optional
        subpass.pDepthStencilAttachment = nullptr;//&depthAttachmentReference;
        subpass.preserveAttachmentCount = 0; // optional
        subpass.pPreserveAttachments = nullptr; // optional

        // subpass dependency is "glue" between different subpasses
        VkSubpassDependency subpassDependency{};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;//|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        //subpassDependency.dependencyFlags

        std::vector<VkAttachmentDescription> attachments{
                colorAttachment,
                //depthAttachment
        };

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &subpassDependency;

        checkResult(vkCreateRenderPass(engine->device, &createInfo, nullptr, &renderPass));
        std::cout << "created render pass" << std::endl;
    }

    RenderPass::~RenderPass() {
        vkDestroyRenderPass(engine->device, renderPass, nullptr);
    }
}