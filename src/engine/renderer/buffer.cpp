#include "vulkan_context.h"
#include "buffer.h"

namespace engine::renderer {

    Buffer::Buffer(size_t size, VkBufferUsageFlags usage) : size(size), allocator(context->allocator) {
        VkBufferCreateInfo bufferInfo{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = size, // size in bytes, should be greater than zero
                .usage = usage,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        VmaAllocationCreateInfo allocationInfo{
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                .usage = VMA_MEMORY_USAGE_AUTO,
                .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        checkResult(vmaCreateBuffer(allocator,
                                    &bufferInfo, &allocationInfo,
                                    &buffer, &allocation, nullptr));
    }

    Buffer::~Buffer() {
//        std::cout << "destroyed buffer" << std::endl;
        vmaDestroyBuffer(allocator, buffer, allocation);
    }

    void Buffer::update(const void *data) {
        void *mappedData;
        vmaMapMemory(allocator, allocation, &mappedData);
        memcpy(mappedData, data, size);
        vmaUnmapMemory(allocator, allocation);
    }

}