#ifndef SPHERE_MEMORY_ALLOCATOR_H
#define SPHERE_MEMORY_ALLOCATOR_H

#include "vulkan_context.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

#include "vk_mem_alloc.h"

#pragma clang diagnostic pop

namespace engine {

    struct AllocatedBufferData {
        VkBuffer buffer;
        VmaAllocation allocation;
    };

    class MemoryAllocator {

    public:
        explicit MemoryAllocator();

        ~MemoryAllocator();

        VmaAllocator allocator;

        template<typename T>
        void createBuffer(VkBuffer &buffer, VmaAllocation &allocation, const T &data, std::size_t size,
                                           VkBufferUsageFlags usageFlags) {
            VkBufferCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createInfo.size = size; // size in bytes, should be greater than zero
            createInfo.usage = usageFlags;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo allocationCreateInfo{};
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            allocationCreateInfo.requiredFlags =
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            allocationCreateInfo.pool = VK_NULL_HANDLE;
            allocationCreateInfo.priority = 1.0f;

            VmaAllocationInfo allocationInfo;
            checkResult(vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &buffer, &allocation,
                                        &allocationInfo));

            void *mappedData;
            vmaMapMemory(allocator, allocation, &mappedData);
            memcpy(mappedData, &data, size);
            vmaUnmapMemory(allocator, allocation);

            AllocatedBufferData allocatedBufferData{
                    .buffer = buffer,
                    .allocation = allocation
            };
            allocatedBuffers.insert({buffer, allocatedBufferData});

            std::cout << "created buffer" << std::endl;
        }

        template<typename T>
        void updateBuffer(const VkBuffer &buffer,
                                           const VmaAllocation &allocation,
                                           const T &data,
                                           const std::size_t &size) {

            void *mappedData;
            vmaMapMemory(allocator, allocation, &mappedData);
            memcpy(mappedData, &data, size);
            vmaUnmapMemory(allocator, allocation);
        }

        template<typename T>
        void updateBuffer(const VkBuffer &buffer,
                                           const T &data,
                                           const std::size_t &size) {
            VmaAllocation &allocation = allocatedBuffers[buffer].allocation;
            updateBuffer(buffer, allocation, data, size);
        }

    private:
        VulkanContext &context;
        std::unordered_map<VkBuffer, AllocatedBufferData> allocatedBuffers;
    };
}

#endif //SPHERE_MEMORY_ALLOCATOR_H
