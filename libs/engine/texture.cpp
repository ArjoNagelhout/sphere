#include "engine.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../stb/stb_image.h"

namespace engine {

    Texture::Texture(const std::string &filePath) : allocator(engine->allocator->allocator) {

        int x, y, channelAmount;
        data = stbi_load(filePath.data(), &x, &y, &channelAmount, 0);
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

        std::unordered_map<int, VkFormat> formats{
                {1, VK_FORMAT_R8_SRGB},
                {2, VK_FORMAT_R8G8_SRGB},
                {3, VK_FORMAT_R8G8B8_SRGB},
                {4, VK_FORMAT_R8G8B8A8_SRGB},
        };

        VkFormat format = formats[channelAmount];

        std::cout << "loaded image" << std::endl;
        std::cout << "x: " << x << ", y: " << y << ", channelAmount: " << channelAmount << ", VkFormat: "
                  << string_VkFormat(format) << std::endl;

        VkExtent3D extent{
                .width = static_cast<uint32_t>(x),
                .height = static_cast<uint32_t>(y),
                .depth = 1
        };

        VkImageCreateInfo imageInfo = vk_create::image(format, extent, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
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
    }

    Texture::~Texture() {
        vmaDestroyImage(allocator, image, allocation);

        stbi_image_free(data);
    }
}