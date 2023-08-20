#define VMA_IMPLEMENTATION
#include "memory.h"
#include "engine.h"

namespace engine::memory {

    VmaAllocator allocator;

    Buffer::Buffer(size_t size, VkBufferUsageFlags usage) : size(size) {
        VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = size; // size in bytes, should be greater than zero
        createInfo.usage = usage;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo{};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocationCreateInfo.requiredFlags =
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        allocationCreateInfo.pool = VK_NULL_HANDLE;
        allocationCreateInfo.priority = 1.0f;

        checkResult(vmaCreateBuffer(allocator,
                                    &createInfo, &allocationCreateInfo,
                                    &buffer, &allocation, nullptr));
    }

    Buffer::~Buffer() {
        vmaDestroyBuffer(allocator, buffer, allocation);
    }

    void Buffer::update(const void *data) {
        void *mappedData;
        vmaMapMemory(allocator, allocation, &mappedData);
        memcpy(mappedData, data, size);
        vmaUnmapMemory(allocator, allocation);
    }

    void initializeAllocator() {
        assert((allocator == nullptr) && "Only one allocator can exist at one time");

        VmaAllocatorCreateInfo allocatorInfo{
                .physicalDevice = engine->physicalDevice,
                .device = engine->device,
                //.preferredLargeHeapBlockSize,
                //.pAllocationCallbacks,
                //.pDeviceMemoryCallbacks,
                .pHeapSizeLimit = nullptr,
                .pVulkanFunctions = nullptr,
                .instance = engine->instance,
        };
        checkResult(vmaCreateAllocator(&allocatorInfo, &allocator));
        std::cout << "created memory allocator" << std::endl;
    }

    void destroyAllocator() {
        vmaDestroyAllocator(allocator);
    }
}