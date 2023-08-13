#ifndef SPHERE_DESCRIPTOR_SET_BUILDER_H
#define SPHERE_DESCRIPTOR_SET_BUILDER_H

#include <vulkan/vulkan.h>
#include <vector>

namespace engine {

    /*
     * Because managing descriptor sets is a major pain, we create a class that manages them.
     *
     * Relevant classes:
     *
     * VkDescriptorPool
     * VkDescriptorSet
     * VkDescriptorSetLayout
     * VkDescriptorSetLayoutBinding
     *
     * Relevant functions:
     *
     * vkCreateDescriptorSetLayout
     * vkDestroyDescriptorSetLayout
     *
     * vkCreateDescriptorPool
     * vkDestroyDescriptorPool
     *
     * vkAllocateDescriptorSets
     * vkFreeDescriptorSets
     * vkResetDescriptorPool (frees all descriptor sets from pool)
     *
     * vkCmdBindDescriptorSets
     * "Each of the pDescriptorSets must be compatible with the pipeline layout specified by layout"
     *
     * VkDescriptorType:
     *
     * VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
     * VK_DESCRIPTOR_TYPE_SAMPLER,
     * VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
     * VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
     * VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
     * VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
     * VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
     * VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
     * VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
     * VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
     * VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
     * VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
     *
     * Uniform block: descriptor and buffer are decoupled
     * Inline uniform block: for certain constants, directly stored in descriptor
     * vs push constants: allows reusing the same data across different drawing and dispatching commands
     *
     * If we want to access descriptor sets from a pipeline, we need to add the descriptor set layout to VkPipelineLayout
     *
     * So, we define a descriptor set layout and register that in the pipeline layout. We then bind the actual descriptor sets
     * during drawing using vkCmdBindDescriptorSets.
     *
     * Pipeline layout and descriptor sets need to be compatible (so created together)
     */
    class DescriptorSetBuilder {

    public:
        explicit DescriptorSetBuilder();
        ~DescriptorSetBuilder();
//            std::vector<VkDescriptorPoolSize> poolSizes{
//                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 }
//            };
//
//            VkDescriptorPoolCreateInfo poolInfo{};
//            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//            poolInfo.maxSets = 1000; // maximum number of sets that can be allocated from the pool
//            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//            poolInfo.pPoolSizes = poolSizes.data();
//        }

        VkDescriptorSetLayout descriptorSetLayout;

        std::vector<VkDescriptorSet> createDescriptorSets(VkDescriptorSetLayout layout, size_t amount);
        void bindBuffer(VkDescriptorSet &descriptorSet, VkBuffer &buffer, uint32_t dstBinding);
        void bindImage(VkDescriptorSet &descriptorSet, VkSampler &sampler, VkImageView &imageView, uint32_t dstBinding);

    private:
        VkDescriptorPool descriptorPool;

        void createDescriptorPool();
        void createDescriptorSetLayout();
    };
}

#endif //SPHERE_DESCRIPTOR_SET_BUILDER_H
