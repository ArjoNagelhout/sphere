#include "scene.h"

namespace engine::renderer {

    Scene::Scene(VkRenderPass renderPass) : renderPass(renderPass) {
        // load meshes
        std::vector<std::string> meshNames{
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/orb.obj",
                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/hollowcube.obj",
//                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/bunny_patched.obj",
//                "/Users/arjonagelhout/Downloads/kenney_platformer-kit/Models/OBJ format/blockSnowRoundedLow.obj",
//                "/Users/arjonagelhout/Downloads/kenney_platformer-kit/Models/OBJ format/ladderBroken.obj",
//                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/bunny.obj",
//                "/Users/arjonagelhout/Documents/ShapeReality/2023-06-18_bgfx_test/bgfx/examples/assets/meshes/column.obj",
//                "/Users/arjonagelhout/Documents/ShapeReality/sphere/external/tinyobjloader/models/map-bump.obj",
        };

        for (const auto &meshName: meshNames) {
            meshes.emplace_back(std::make_unique<Mesh>(meshName));
        }

        // load images
        struct TextureData {
            std::string filePath;
        };

        std::vector<TextureData> texturesData{
                {"/Users/arjonagelhout/Documents/ShapeReality/2023-06-11_green_assets/textures/edited/leaves_1.png"},
                {"/Users/arjonagelhout/Documents/ShapeReality/2023-06-11_green_assets/textures/edited/bench.png"},
                {"/Users/arjonagelhout/Documents/ShapeReality/2023-06-11_green_assets/textures/edited/bark_1.png"},
        };

        for (const auto &textureData : texturesData) {
            textures.emplace_back(std::make_unique<Texture>(textureData.filePath));
        }

        // load shaders
        struct ShaderData {
            std::string vertexShaderPath;
            std::string fragmentShaderPath;
        };

        std::vector<ShaderData> shadersData{
                {"shader_vert.spv",      "shader_frag.spv"},
                {"shader_test_vert.spv", "shader_test_frag.spv"}
        };

        for (const auto &shaderData: shadersData) {
            shaders.emplace_back(std::make_unique<Shader>(shaderData.vertexShaderPath, shaderData.fragmentShaderPath, renderPass)); // todo: stupid, remove renderpass argument
        }

        // create scene with objects
        struct ObjectData {
            std::string name;
            glm::vec3 position;
            glm::vec3 scale;
            Mesh &mesh;
            Material &material;
        };

        struct MaterialData {
            Shader &shader;
            Texture &texture;
        };

        std::vector<MaterialData> materialsData{
                {*shaders[0], {*textures[0]}},
                {*shaders[1], {*textures[0]}},
                {*shaders[0], {*textures[1]}},
                {*shaders[0], {*textures[2]}},
        };

        for (const auto &materialData: materialsData) {
            materials.emplace_back(std::make_unique<Material>(materialData.shader, materialData.texture));
            const auto &mat = materials.back();
        }

        std::vector<ObjectData> objectsData{
                {"Wee", {0, 0,  0}, {1,   1,    1},    *meshes[0], *materials[0]},
                {"Dingetje", {0, 2,  0}, {0.9, 1,    1},    *meshes[0], *materials[1]},
                {"Objectje", {0, 4,  0}, {0.8, 1,    1},    *meshes[0], *materials[2]},
                {"Woeie", {0, 6,  0}, {0.7, 1,    1},    *meshes[0], *materials[3]},
                {"Bowo", {-4, 0,  0}, {0.6, 1,    1},    *meshes[0], *materials[1]},
                {"Meet", {-4, 2, 0}, {0.5, 1,    1},    *meshes[0], *materials[2]},
                {"Frie", {-4, 4, 0}, {0.4, 1,    1},    *meshes[0], *materials[1]},
                {"Sat", {-4, 6, 0}, {0.3, 1,    1},    *meshes[0], *materials[0]},
                {"Fry", {4, 0,  0}, {1,   0.25, 0.25}, *meshes[1], *materials[1]},
                {"Lalalal33", {4, 2,  0}, {0.9, 0.25, 0.25}, *meshes[1], *materials[1]},
                {"1Wee", {4, 4,  0}, {0.8, 0.25, 0.25}, *meshes[1], *materials[1]},
                {"Obje223", {4, 6,  0}, {0.7, 0.25, 0.25}, *meshes[1], *materials[0]},
                {"Llsss", {4, 8,  0}, {0.6, 0.25, 0.25}, *meshes[1], *materials[0]},
                {"Llalalal", {4, 10, 0}, {0.5, 0.25, 0.25}, *meshes[1], *materials[0]},
                {"MBes", {4, 12, 0}, {0.4, 0.25, 0.25}, *meshes[1], *materials[0]},
                {"Ke3", {4, 14, 0}, {0.3, 0.25, 0.25}, *meshes[1], *materials[0]},
        };

        for (const auto &objectData: objectsData) {
            objects.emplace_back(std::make_unique<Object>(objectData.name, objectData.mesh, objectData.material));
            const auto &obj = objects.back();
            obj->localPosition = objectData.position;
            obj->localScale = objectData.scale;
        }
    }

    Scene::~Scene() = default;

    void Scene::update() {
        // update mesh transforms
        for (size_t i = 0; i < objects.size(); i++) {
            const auto &object = objects[i];
            object->localRotation *=
                    glm::angleAxis(0.05f * (-1.0f + 2.0f * static_cast<float>(i % 2)),
                                   glm::vec3(0, 1, 0));
        }
    }
}