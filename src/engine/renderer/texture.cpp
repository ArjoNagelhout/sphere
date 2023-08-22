#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "vulkan_context.h"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

namespace engine::renderer {

    Texture::Texture(const std::string &filePath) {
        int x, y, channelAmount;
        data = stbi_load(filePath.data(), &x, &y, &channelAmount, STBI_rgb_alpha); // forces 4 8-bit components per pixel
        // channelAmount will be the original value if it was not forced.

        if (data == NULL) {
            const char *reason = stbi_failure_reason();
            std::cout << reason << std::endl;
        }

        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

        std::cout << "loaded image at: " << filePath << std::endl;
        std::cout << "x: " << x << ", y: " << y << ", channelAmount: " << channelAmount << ", VkFormat: "
                  << string_VkFormat(format) << std::endl;

        VkExtent3D extent{
                .width = static_cast<uint32_t>(x),
                .height = static_cast<uint32_t>(y),
                .depth = 1
        };

        VkDeviceSize sizeInBytes = x * y * 4;//channelAmount; channelAmount is still the original amount instead of the forced 4 channels.

        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;
        VkBufferCreateInfo stagingBufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = sizeInBytes,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };
        VmaAllocationCreateInfo stagingBufferAllocationInfo{
            //.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            .usage = VMA_MEMORY_USAGE_CPU_ONLY,
            //.requiredFlags = VK_MEMORY_PROPERTY
        };
        checkResult(vmaCreateBuffer(context->allocator, &stagingBufferInfo, &stagingBufferAllocationInfo, &stagingBuffer, &stagingBufferAllocation, nullptr));

        std::cout << "created staging buffer" << std::endl;

        void *mappedData;
        vmaMapMemory(context->allocator, stagingBufferAllocation, &mappedData);
        memcpy(mappedData, data, static_cast<size_t>(sizeInBytes));
        //vmaFlushAllocation(allocator, stagingBufferAllocation, 0, VK_WHOLE_SIZE);
        vmaUnmapMemory(context->allocator, stagingBufferAllocation);
        stbi_image_free(data);

        std::cout << "copied data into staging buffer" << std::endl;

        VkImageCreateInfo imageInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = format,
                .extent = extent,
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // VK_IMAGE_LAYOUT_UNDEFINED
        };
        VmaAllocationCreateInfo allocationInfo{
                .flags = 0,
                .usage = VMA_MEMORY_USAGE_GPU_ONLY,
                .requiredFlags = 0,
        };
        checkResult(vmaCreateImage(context->allocator, &imageInfo, &allocationInfo, &image, &allocation, nullptr));

        context->uploadContext->submit([&](VkCommandBuffer cmd) {

            // set image layout to "transfer destination optimal"
            uint32_t queueFamilyIndex = context->queueFamiliesData.graphicsQueueFamilyData->index;
            VkImageMemoryBarrier barrierToTransferDst{
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    .srcQueueFamilyIndex = queueFamilyIndex,
                    .dstQueueFamilyIndex = queueFamilyIndex,
                    .image = image,
                    .subresourceRange = {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1
                    }
            };

            vkCmdPipelineBarrier(cmd,
                                 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_DEPENDENCY_DEVICE_GROUP_BIT,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrierToTransferDst);

            // copy from the buffer to the image
            VkBufferImageCopy copyRegion{
                    .bufferOffset = 0,
                    .bufferRowLength = 0,
                    .bufferImageHeight = 0,
                    .imageSubresource = {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .mipLevel = 0,
                            .baseArrayLayer = 0,
                            .layerCount = 1,
                    },
                    .imageOffset = {0, 0, 0},
                    .imageExtent = extent
            };
            vkCmdCopyBufferToImage(cmd, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            // change image layout to "shader read optimal"
            VkImageMemoryBarrier barrier{
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    .srcQueueFamilyIndex = queueFamilyIndex,
                    .dstQueueFamilyIndex = queueFamilyIndex,
                    .image = image,
                    .subresourceRange = {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1
                    }
            };

            vkCmdPipelineBarrier(cmd,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 VK_DEPENDENCY_DEVICE_GROUP_BIT,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);
        });

        // image view
        VkImageViewCreateInfo imageViewInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
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
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                }
        };

        checkResult(vkCreateImageView(context->device, &imageViewInfo, nullptr, &imageView));

        std::cout << "created image view" << std::endl;

        // sampler
        VkSamplerCreateInfo samplerInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_NEVER,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
                .unnormalizedCoordinates = VK_FALSE
        };

        checkResult(vkCreateSampler(context->device, &samplerInfo, nullptr, &sampler));

        std::cout << "created sampler" << std::endl;

        vmaDestroyBuffer(context->allocator, stagingBuffer, stagingBufferAllocation);

        std::cout << "destroyed staging buffer" << std::endl;

        std::cout << "created texture" << std::endl;
    }

    Texture::~Texture() {
        vkDestroySampler(context->device, sampler, nullptr);
        vkDestroyImageView(context->device, imageView, nullptr);
        vmaDestroyImage(context->allocator, image, allocation);

//        std::cout << "destroyed texture" << std::endl;
    }
}