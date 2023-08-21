#include "vulkan_context.h"
#include "material.h"
#include "descriptor_set_builder.h"

namespace engine::renderer {
    Material::Material(const Shader &shader, Texture &texture) : shader(shader), texture(texture) {

        // set descriptor sets
        descriptorSet = descriptorSetBuilder->createDescriptorSets(shader.descriptorSetLayout, 1)[0];
        bindImage(descriptorSet, texture.sampler, texture.imageView, 1);
    }

    Material::~Material() = default;
}