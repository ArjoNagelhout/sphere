#ifndef SPHERE_TYPES_H
#define SPHERE_TYPES_H

#include "glm.h"

namespace engine {

    struct VertexAttributes {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
    };
}

#endif //SPHERE_TYPES_H
