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

    public:
        explicit MemoryAllocator(Device &device) {
            createMemoryAllocator(device, allocator);
        }

        ~MemoryAllocator() {
            vmaDestroyAllocator(allocator);
        }

        const VmaAllocator &getAllocator() {
            return allocator;
        }

    private:
        VmaAllocator allocator;

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
