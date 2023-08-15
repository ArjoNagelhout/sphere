#ifndef SPHERE_MESH_H
#define SPHERE_MESH_H

#include <vector>
#include "types.h"

namespace engine {

    class Mesh{

    public:
        explicit Mesh();
        ~Mesh();

        std::vector<VertexAttributes> vertices;
        std::vector<int> indices;

    private:
        // should contain a buffer with data for vertices and indices
    };
}

#endif //SPHERE_MESH_H
