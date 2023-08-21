#ifndef SPHERE_SCENE_H
#define SPHERE_SCENE_H

#include <vector>

#include "object.h"
#include "texture.h"

namespace engine::renderer {

    /*
     * A scene contains a list of objects that can be drawn to the screen,
     * they should be sorted / grouped by their shader / pipeline object
     */
    class Scene {

    public:
        explicit Scene(VkRenderPass renderPass);
        ~Scene();

        std::vector<std::unique_ptr<Object>> objects;
        std::vector<std::unique_ptr<Material>> materials;

        // todo: refactor out
        void update();

    private:
        // todo: stupid, refactor engine into editor so that we don't have to pass this into the scene.
        VkRenderPass renderPass;

        std::vector<std::unique_ptr<Texture>> textures;
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::unique_ptr<Shader>> shaders;
    };
}

#endif //SPHERE_SCENE_H