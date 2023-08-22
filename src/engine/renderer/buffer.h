#ifndef SPHERE_BUFFER_H
#define SPHERE_BUFFER_H

#include "utils.h"

#include "vma.h"

#include <iostream>

namespace engine::renderer {

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
