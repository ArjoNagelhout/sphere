#ifndef SPHERE_OBJECT_H
#define SPHERE_OBJECT_H

#include "mesh.h"
#include "material.h"

#include <glm/mat4x4.hpp>

namespace engine {

    /*
     * An object contains a mesh and a material that is used to render that mesh
     *
     * In the future, this can be refactored to be more modular etc.
     */
    class Object {

    public:
        explicit Object(Mesh &mesh, Material &material);
        ~Object();

        Mesh &mesh;
        Material &material;

        glm::mat4 transform;

    private:

    };
}

#endif //SPHERE_OBJECT_H
