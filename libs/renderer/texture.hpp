#ifndef SPHERE_TEXTURE_HPP
#define SPHERE_TEXTURE_HPP

#include "device.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace renderer {

    /*
     * A Texture that is present on the GPU
     */
    class Texture {

        // relevant classes:
        // VkImage
        // VkBuffer
        // VkSampler
        // VkImageView
        // VkBufferView
        // VkFramebuffer

        // pipeline barriers
        // memory barriers

        explicit Texture(Device &device) {

            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            //imageInfo.flags
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
            //imageInfo.extent
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            //imageInfo.queueFamilyIndexCount
            //imageInfo.pQueueFamilyIndices
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkImage image;

            VkResult result = vkCreateImage(device.getDevice(), &imageInfo, nullptr, &image);


        }

        ~Texture() {
            // deallocate buffer
        }
    };

}

#endif //SPHERE_TEXTURE_HPP
