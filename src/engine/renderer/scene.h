#ifndef SPHERE_SCENE_H
#define SPHERE_SCENE_H

#include <vector>

#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"

#include "material_system.h"
#include "texture.h"
#include "mesh.h"

namespace engine::renderer {

    /*
     * An object contains a mesh and a material that is used to render that mesh
     *
     * In the future, this can be refactored to be more modular etc.
     */
    class Object {

    public:
        explicit Object(const std::string &name, Mesh &mesh, Material &material);
        ~Object();

        std::string name;
        Mesh &mesh;
        Material &material; // the shader that is used for rendering this object, todo: should be replaced by Material

        // todo: should be refactored out into Transform component using ECS
        glm::vec3 localPosition{0, 0, 0};
        glm::quat localRotation{0, 0, 0, 1}; // identity quaternion, otherwise multiplication always results in 0
        glm::vec3 localScale{1, 1, 1};
        glm::mat4 getTransform();

    private:

    };

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