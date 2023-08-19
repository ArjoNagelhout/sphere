#ifndef SPHERE_MESH_H
#define SPHERE_MESH_H

#include "memory_allocator.h"

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

#include <vector>
#include "types.h"

namespace engine {

    class Mesh{

    public:
        explicit Mesh(const std::string &filePath);
        ~Mesh();

        std::vector<VertexAttributes> vertices{
                {{-1.0f, -1.0f, 0}, {0, 0}, {1, 0, 0}},
                {{1.0f,  -1.0f, 0}, {0, 0}, {0, 1, 0}},
                {{1.0f,  1.0f,  0}, {0, 0}, {0, 0, 1}},
                {{-1.0f, 1.0f,  0}, {0, 0}, {1, 1, 1}}
        };
        std::vector<uint32_t> indices{
                0, 1, 2, 2, 3, 0
        };

        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;

    private:
        void loadObj(const std::string &filePath);

        MemoryAllocator &allocator;

        // buffers
        VmaAllocation vertexBufferAllocation;
        VmaAllocation indexBufferAllocation;
    };
}

#endif //SPHERE_MESH_H
