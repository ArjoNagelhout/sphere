#include "engine.h"
#include "material.h"

namespace engine {
    Material::Material(const Shader &shader, Texture &texture) : shader(shader), texture(texture) {

        // set descriptor sets
        DescriptorSetBuilder &builder = *engine->descriptorSetBuilder;
        descriptorSets = builder.createDescriptorSets(shader.descriptorSetLayout, 1);
        bindBuffer(descriptorSets[0], engine->camera->cameraDataBuffer, 0);
        bindImage(descriptorSets[0], texture.sampler, texture.imageView, 1);
    }

    Material::~Material() = default;
}