#ifndef SPHERE_MEMORY_ALLOCATOR_HPP
#define SPHERE_MEMORY_ALLOCATOR_HPP

#include "device.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#define VMA_IMPLEMENTATION

#include "vk_mem_alloc.h"

#pragma clang diagnostic pop

namespace renderer {

    class MemoryAllocator {

    private:
        struct AllocatedBufferData {
            VkBuffer buffer;
            VmaAllocation allocation;
        };

    public:
        explicit MemoryAllocator(Device &device) {
            createMemoryAllocator(device, allocator);
        }

        ~MemoryAllocator() {

            // destroy allocated buffers
            for (auto const &allocatedBuffer : allocatedBuffers) {
                const AllocatedBufferData &data = allocatedBuffer.second;

                vmaDestroyBuffer(allocator, data.buffer, data.allocation);

                std::cout << "destroyed buffer" << std::endl;
            }

            vmaDestroyAllocator(allocator);
        }

        const VmaAllocator &getAllocator() {
            return allocator;
        }

        template<typename T>
        void createBuffer(VkBuffer &buffer,
                          VmaAllocation &allocation,
                          const T &data,
                          std::size_t size,
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
            VkResult result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &buffer, &allocation,
                                              &allocationInfo);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create buffer: ") + string_VkResult(result));
            }

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

    private:
        VmaAllocator allocator;

        std::unordered_map<VkBuffer, AllocatedBufferData> allocatedBuffers;

        static void createMemoryAllocator(Device &device, VmaAllocator &allocator) {

            VmaAllocatorCreateInfo allocatorInfo{};
            allocatorInfo.physicalDevice = device.getPhysicalDevice();
            allocatorInfo.device = device.getDevice();
            //allocatorInfo.preferredLargeHeapBlockSize
            //allocatorInfo.pAllocationCallbacks
            //allocatorInfo.pDeviceMemoryCallbacks
            allocatorInfo.pHeapSizeLimit = nullptr;
            allocatorInfo.pVulkanFunctions = nullptr;
            allocatorInfo.instance = device.getInstance();

            VkResult result = vmaCreateAllocator(&allocatorInfo, &allocator);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create allocator: ") + string_VkResult(result));
            }

            std::cout << "created memory allocator" << std::endl;
        }
    };

}

#endif //SPHERE_MEMORY_ALLOCATOR_HPP
