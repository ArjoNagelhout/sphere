#ifndef SPHERE_OBJECT_H
#define SPHERE_OBJECT_H

#include "mesh.h"
#include "material.h"
#include "shader.h"

#include <glm/mat4x4.hpp>
#include<glm/gtx/quaternion.hpp>

namespace engine {

    /*
     * An object contains a mesh and a material that is used to render that mesh
     *
     * In the future, this can be refactored to be more modular etc.
     */
    class Object {

    public:
        explicit Object(Mesh &mesh, Shader &shader); //Material &material);
        ~Object();

        Mesh &mesh;
        Shader &shader; // the shader that is used for rendering this object, todo: should be replaced by Material

        // todo: should be refactored out into Transform component using ECS
        glm::vec3 localPosition;
        glm::quat localRotation{0, 0, 0, 1}; // identity quaternion, otherwise multiplication always results in 0
        glm::vec3 localScale;
        glm::mat4 getTransform();

    private:

    };
}

#endif //SPHERE_OBJECT_H
