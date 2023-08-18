#include "object.h"

#include<glm/gtx/transform.hpp>

namespace engine {

    Object::Object(Mesh &mesh, Shader &shader) : mesh(mesh), shader(shader) {

    }

    Object::~Object() = default;

    glm::mat4x4 Object::getTransform() {
        // calculates the transform from the position, rotation and scale

        glm::mat4x4 translateMatrix{glm::translate(localPosition)};
        glm::mat4x4 rotateMatrix{glm::toMat4(localRotation)};
        glm::mat4x4 scaleMatrix{glm::scale(localScale)};

        return translateMatrix * rotateMatrix * scaleMatrix;
    }
}