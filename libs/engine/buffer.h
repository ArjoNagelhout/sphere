#ifndef SPHERE_BUFFER_H
#define SPHERE_BUFFER_H

#define VK_ENABLE_BETA_EXTENSIONS

#include "vulkan/vulkan.h"
#include "utils.h"
#include <memory>
#include <iostream>

namespace engine {

    class Buffer {

    public:
        explicit Buffer(size_t size, VkBufferUsageFlags usage);
        ~Buffer();

        VkBuffer buffer;

        void update(const void *data);

    private:
        VmaAllocator allocator;
        VmaAllocation allocation;
        size_t size;
    };
}

#endif //SPHERE_BUFFER_H
