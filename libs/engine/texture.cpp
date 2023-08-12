#include "engine.h"
#include "texture.h"

namespace engine {

    Texture::Texture(const std::string &filePath, const VkFormat &format, const VkExtent2D &extent) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        //imageInfo.flags
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent = toExtent3D(extent);
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //imageInfo.queueFamilyIndexCount
        //imageInfo.pQueueFamilyIndices
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocationInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        allocationInfo.pool = VK_NULL_HANDLE;
        allocationInfo.priority = 1.0f;

        checkResult(vmaCreateImage(engine->allocator->allocator, &imageInfo, &allocationInfo, &image, &allocation, nullptr));
    }

    Texture::~Texture() {
        vmaDestroyImage(engine->allocator->allocator, image, allocation);
    }
}