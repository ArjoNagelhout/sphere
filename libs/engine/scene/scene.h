#ifndef SPHERE_SCENE_H
#define SPHERE_SCENE_H

#include <vector>

#include "object.h"
#include "renderer/texture.h"

namespace engine {

    /*
     * A scene contains a list of objects that can be drawn to the screen,
     * they should be sorted / grouped by their shader / pipeline object
     */
    class Scene {

    public:
        explicit Scene();
        ~Scene();

        std::vector<std::unique_ptr<Object>> objects;

        // todo: refactor out
        void update();

    private:
        std::vector<std::unique_ptr<Texture>> textures;
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::unique_ptr<Material>> materials;
        std::vector<std::unique_ptr<Shader>> shaders;
    };
}

#endif //SPHERE_SCENE_H