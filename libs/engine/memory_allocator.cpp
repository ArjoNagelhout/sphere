#define VMA_IMPLEMENTATION
#include "memory_allocator.h"

namespace engine {

    MemoryAllocator::MemoryAllocator() : context(getContext()) {

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = context.physicalDevice;
        allocatorInfo.device = context.device;
        //allocatorInfo.preferredLargeHeapBlockSize
        //allocatorInfo.pAllocationCallbacks
        //allocatorInfo.pDeviceMemoryCallbacks
        allocatorInfo.pHeapSizeLimit = nullptr;
        allocatorInfo.pVulkanFunctions = nullptr;
        allocatorInfo.instance = context.instance;

        checkResult(vmaCreateAllocator(&allocatorInfo, &allocator));
        std::cout << "created memory allocator" << std::endl;
    }

    MemoryAllocator::~MemoryAllocator() {

        // destroy allocated buffers
        for (auto const &allocatedBuffer: allocatedBuffers) {
            const AllocatedBufferData &data = allocatedBuffer.second;

            vmaDestroyBuffer(allocator, data.buffer, data.allocation);

            std::cout << "destroyed buffer" << std::endl;
        }

        vmaDestroyAllocator(allocator);
    }
}