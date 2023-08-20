#ifndef SPHERE_VULKAN_CREATE_H
#define SPHERE_VULKAN_CREATE_H

#include <vulkan/vulkan.h>
#include <vector>

namespace engine::vk_create {

    // if a Vk___CreateInfo is used often and often with the same parameters, it can be defined here

    VkImageCreateInfo image(const VkFormat &format, const VkExtent3D &extent, const VkImageUsageFlags &usage);
    VkImageViewCreateInfo imageView(const VkImage &image, const VkFormat &format, const VkImageAspectFlags &aspectMask);
    VkFramebufferCreateInfo framebuffer(const VkRenderPass &renderPass, const std::vector<VkImageView> &attachments, const VkExtent2D &extent);
}

#endif //SPHERE_VULKAN_CREATE_H
