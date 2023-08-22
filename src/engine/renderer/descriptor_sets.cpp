#include "vulkan_context.h"
#include "descriptor_sets.h"

#include <cassert>
#include <iostream>

namespace engine::renderer {

    DescriptorSetBuilder *descriptorSetBuilder;

    DescriptorSetBuilder::DescriptorSetBuilder() {
        assert((descriptorSetBuilder == nullptr) && "Only one descriptor set builder can exist at one time");
        descriptorSetBuilder = this;

        createDescriptorPool();
    }

    DescriptorSetBuilder::~DescriptorSetBuilder() {
        vkDestroyDescriptorPool(context->device, descriptorPool, nullptr);
    }

    /*
     * Creates a descriptor set layout from a list of VkDescriptorSetLayoutBindings.
     * No additional data is assumed or required
     */
    VkDescriptorSetLayout createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings) {
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = static_cast<uint32_t>(bindings.size()),
                .pBindings = bindings.data(),
        };
        checkResult(vkCreateDescriptorSetLayout(context->device,
                                                &descriptorSetLayoutInfo,
                                                nullptr, &descriptorSetLayout));
        return descriptorSetLayout;
    }

    VkDescriptorSetLayout createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding cameraData{
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
        };

        VkDescriptorSetLayoutBinding diffuse{
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
        };

        std::vector<VkDescriptorSetLayoutBinding> bindings{
                cameraData,
                diffuse
        };

        return createDescriptorSetLayout(bindings);
    }

    void DescriptorSetBuilder::createDescriptorPool() {
        std::vector<VkDescriptorPoolSize> poolSizes{
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        };

        VkDescriptorPoolCreateInfo poolInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = 1000,
                .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
                .pPoolSizes = poolSizes.data(),
        };
        checkResult(vkCreateDescriptorPool(context->device, &poolInfo, nullptr, &descriptorPool));
        std::cout << "created descriptor pool" << std::endl;
    }

    /*
     * Creates descriptor sets with a given amount of sets per layout
     *
     * input: layouts = [L1, L2, L3], amount = 2
     * output: [L1, L2, L3, L1, L2, L3]
     */
    std::vector<VkDescriptorSet>
    DescriptorSetBuilder::createDescriptorSets(VkDescriptorSetLayout layout, size_t amount) {

        std::vector<VkDescriptorSet> descriptorSets(amount);
        std::vector<VkDescriptorSetLayout> layouts(amount, layout);

        VkDescriptorSetAllocateInfo allocateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = descriptorPool,
                .descriptorSetCount = static_cast<uint32_t>(amount),
                .pSetLayouts = layouts.data(),
        };
        checkResult(vkAllocateDescriptorSets(context->device, &allocateInfo, descriptorSets.data()));
        std::cout << "created descriptor sets" << std::endl;

        return descriptorSets;
    }

    void bindImage(VkDescriptorSet &descriptorSet, VkSampler &sampler, VkImageView &imageView, uint32_t dstBinding) {
        VkDescriptorImageInfo imageInfo{
                .sampler = sampler,
                .imageView = imageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkWriteDescriptorSet writeInfo{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = dstBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr,
        };
        vkUpdateDescriptorSets(context->device, 1, &writeInfo, 0, nullptr);
    }

    void bindBuffer(VkDescriptorSet &descriptorSet, VkBuffer &buffer, uint32_t dstBinding) {
        VkDescriptorBufferInfo bufferInfo{
                .buffer = buffer,
                .offset = 0,
                .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeInfo{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = dstBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &bufferInfo,
                .pTexelBufferView = nullptr,
        };
        vkUpdateDescriptorSets(context->device, 1, &writeInfo, 0, nullptr);
    }
}