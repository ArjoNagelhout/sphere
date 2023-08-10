#define VMA_IMPLEMENTATION
#include "memory_allocator.hpp"

namespace renderer {

    MemoryAllocator::MemoryAllocator() : engine(getEngine()) {

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = engine.physicalDevice;
        allocatorInfo.device = engine.device;
        //allocatorInfo.preferredLargeHeapBlockSize
        //allocatorInfo.pAllocationCallbacks
        //allocatorInfo.pDeviceMemoryCallbacks
        allocatorInfo.pHeapSizeLimit = nullptr;
        allocatorInfo.pVulkanFunctions = nullptr;
        allocatorInfo.instance = engine.instance;

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