#include "engine.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../stb/stb_image.h"

namespace engine {

    Texture::Texture(const std::string &filePath) : allocator(engine->allocator->allocator) {

        int x, y, channelAmount;
        data = stbi_load(filePath.data(), &x, &y, &channelAmount, STBI_rgb_alpha);
        // ... process data if not NULL ...
        // ... x = width, y = height, n = # 8-bit components per pixel ...
        // ... replace '0' with '1'..'4' to force that many components per pixel
        // ... but 'n' will always be the number that it would have been if you said 0

        //       N=#comp     components
        //       1           grey
        //       2           grey, alpha
        //       3           red, green, blue
        //       4           red, green, blue, alpha

        if (data == NULL) {
            const char *reason = stbi_failure_reason();
            std::cout << reason << std::endl;
        }

//        std::unordered_map<int, VkFormat> formats{
//                {1, VK_FORMAT_R8_SRGB},
//                {2, VK_FORMAT_R8G8_SRGB},
//                {3, VK_FORMAT_R8G8B8_SRGB},
//                {4, VK_FORMAT_R8G8B8A8_SRGB},
//        };
//
//        VkFormat format = formats[channelAmount];

        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

        std::cout << "loaded image at: " << filePath << std::endl;
        std::cout << "x: " << x << ", y: " << y << ", channelAmount: " << channelAmount << ", VkFormat: "
                  << string_VkFormat(format) << std::endl;

        VkExtent3D extent{
                .width = static_cast<uint32_t>(x),
                .height = static_cast<uint32_t>(y),
                .depth = 1
        };

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
                .usage = VK_IMAGE_USAGE_SAMPLED_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // VK_IMAGE_LAYOUT_UNDEFINED
        };
        VmaAllocationCreateInfo allocationInfo{
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                .usage = VMA_MEMORY_USAGE_AUTO,
                .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };
        checkResult(vmaCreateImage(allocator, &imageInfo, &allocationInfo, &image, &allocation, nullptr));

        void *mappedData;
        vmaMapMemory(allocator, allocation, &mappedData);
        memcpy(mappedData, &data, x * y * channelAmount);
        vmaUnmapMemory(allocator, allocation);

        std::cout << "uploaded data" << std::endl;

        stbi_image_free(data);

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

        checkResult(vkCreateImageView(engine->device, &imageViewInfo, nullptr, &imageView));

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

        checkResult(vkCreateSampler(engine->device, &samplerInfo, nullptr, &sampler));

        const VkCommandBuffer &cmd = engine->uploadCommandBuffer;

        // upload the image to the read only shader layout
        checkResult(vkResetCommandPool(engine->device, engine->commandPool, 0));
        VkCommandBufferBeginInfo beginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        checkResult(vkBeginCommandBuffer(engine->uploadCommandBuffer, &beginInfo));

        VkImageCopy imageCopy{
                .srcSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 1,
                        .baseArrayLayer = 1,
                        .layerCount = 1
                },
                .srcOffset = {0, 0, 0},
                .dstSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 1,
                        .baseArrayLayer = 1,
                        .layerCount = 1
                },
                .dstOffset = {0, 0, 0},
                .extent = extent,
        };

        VkImage targetImage;
        VkImageCreateInfo targetImageInfo{
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
                .usage = VK_IMAGE_USAGE_SAMPLED_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // VK_IMAGE_LAYOUT_UNDEFINED
        };

        checkResult(vkCreateImage(engine->device, &targetImageInfo, nullptr, &targetImage));

        std::cout << "created target image" << std::endl;

        vkCmdCopyImage(cmd, image, VK_IMAGE_LAYOUT_UNDEFINED, targetImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, &imageCopy);


        VkSubmitInfo submitInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = nullptr,
                .commandBufferCount = 1,
                .pCommandBuffers = &cmd,
        };
        checkResult(vkEndCommandBuffer(cmd));
        checkResult(vkQueueSubmit(engine->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
        vkDeviceWaitIdle(engine->device);

        std::cout << "copied image" << std::endl;

        std::cout << "created texture" << std::endl;
    }

    Texture::~Texture() {

        vkDestroySampler(engine->device, sampler, nullptr);
        vkDestroyImageView(engine->device, imageView, nullptr);
        vmaDestroyImage(allocator, image, allocation);
    }
}