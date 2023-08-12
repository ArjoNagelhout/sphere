#include "vulkan_create.h"

namespace engine::vk_create {

    VkImageCreateInfo image(const VkFormat &format, const VkExtent3D &extent, const VkImageUsageFlags &usage) {
        VkImageCreateInfo info{
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .pNext = nullptr,
                //info.flags,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = format,
                .extent = extent,
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = usage,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                //imageInfo.queueFamilyIndexCount,
                //imageInfo.pQueueFamilyIndices,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        return info;
    }

    VkImageViewCreateInfo imageView(const VkImage &image, const VkFormat &format, const VkImageAspectFlags &aspectMask) {
        VkImageViewCreateInfo info{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = format,
                .components = {
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                        .aspectMask = aspectMask,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                }
        };
        return info;
    }

    VkFramebufferCreateInfo framebuffer(const VkRenderPass &renderPass, const std::vector<VkImageView> &attachments, const VkExtent2D &extent) {
        VkFramebufferCreateInfo info{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = renderPass,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments = attachments.data(),
                .width = extent.width,
                .height = extent.height,
                .layers = 1
        };
        return info;
    }
}