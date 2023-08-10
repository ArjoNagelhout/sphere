#ifndef SPHERE_OBJECT_H
#define SPHERE_OBJECT_H

#include "mesh.h"
#include "material.h"

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

    private:
        Mesh &mesh;
        Material &material;
    };
}

#endif //SPHERE_OBJECT_H
