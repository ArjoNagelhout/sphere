#ifndef SPHERE_MESH_H
#define SPHERE_MESH_H

#include "core/includes.h"

#include "renderer/buffer.h"
#include "core/types.h"

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

        std::unique_ptr<renderer::Buffer> vertexBuffer;
        std::unique_ptr<renderer::Buffer> indexBuffer;

    private:
        void loadObj(const std::string &filePath);
    };
}

#endif //SPHERE_MESH_H
