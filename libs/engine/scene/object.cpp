#include "object.h"

namespace engine {

    Object::Object(Mesh &mesh, Material &material) : mesh(mesh), material(material) {

    }

    Object::~Object() {

    }
}