#ifndef SPHERE_TYPES_H
#define SPHERE_TYPES_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-volatile"

#include <glm/glm.hpp>

#pragma clang diagnostic pop

namespace engine {

    struct VertexAttributes {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
    };
}

#endif //SPHERE_TYPES_H
