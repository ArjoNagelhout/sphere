#ifndef SPHERE_MATERIAL_H
#define SPHERE_MATERIAL_H

#include "shader.h"
#include "texture.h"

#include <vulkan/vulkan.h>

namespace engine::renderer {

    /*
     * A material contains a reference to a shader and contains the properties such as
     * a texture.
     */
    class Material {

    public:
        explicit Material(const Shader &shader, Texture &texture);
        ~Material();

        const Shader &shader;
        renderer::Texture &texture;

        VkDescriptorSet descriptorSet;

    private:

    };

}

#endif //SPHERE_MATERIAL_H
