#ifndef SPHERE_TEXTURE_H
#define SPHERE_TEXTURE_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace engine {

    /*
     * relevant classes:
     * VkImage
     * VkBuffer
     * VkSampler
     * VkImageView
     * VkBufferView
     * VkFramebuffer
     *
     * pipeline barriers
     * memory barriers
     *
     * samplers should be set in the fragment shader. How?
     * using descriptor sets
     *
     * how do we bind the graphics pipeline to a given shader?
     * it seems all rather hard coded.
     * can we use a written shader as the basis for creating the
     * pipeline.
     *
     * how many times can we switch between pipelines, do we have to sort them?
     * we have to use a pipeline cache
     *
     * do we want a texture that is both on the CPU and the GPU?
     * or do we upload it to the GPU and leave it there?
     *
     * maybe a distinction similar to RenderTextures in Unity.
     *
     * We need some way of converting the input data: .png, .jpg, .tif, .bmp
     * to the output data: a binary representation in RAM.
     *
     * we then want to upload it to the gpu.
     *
     * we should also store a cached version on disk that is already in a binary format
     * so that we don't have incredibly slow load times each time we open the scene.
     *
     * this way we can have both quick startup times, and the ability to manipulate / change textures
     * at runtime by modifying the source files (.png, .jpg etc)
     *
     * the only issue is that it's quite hard to manage these different representations, while not increasing memory usage
     * for each representation of the image.
     *
     * we start with the simplest implementation, with a file path as the argument
     */
    class Texture {

    public:
        explicit Texture(const std::string &filePath);
        ~Texture();

    private:
        VmaAllocator &allocator;
        unsigned char *data;
        VkImage image;
        VkImageView imageView;
        VmaAllocation allocation;
    };

}

#endif //SPHERE_TEXTURE_H
