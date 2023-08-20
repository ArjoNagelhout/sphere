#ifndef SPHERE_MEMORY_H
#define SPHERE_MEMORY_H

#define VK_ENABLE_BETA_EXTENSIONS

#include "vulkan/vulkan.h"
#include "utils.h"
#include <memory>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

#include "vk_mem_alloc.h"

#pragma clang diagnostic pop

namespace engine::memory {

    extern VmaAllocator allocator;
    void initializeAllocator();
    void destroyAllocator();

    class Buffer {

    public:
        explicit Buffer(size_t size, VkBufferUsageFlags usage);
        ~Buffer();

        VkBuffer buffer;

        void update(const void *data);

    private:
        VmaAllocation allocation;
        size_t size;
    };
}

#endif //SPHERE_MEMORY_H
