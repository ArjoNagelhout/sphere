#define VMA_IMPLEMENTATION
#include "memory_allocator.h"
#include "engine.h"

namespace engine {

    MemoryAllocator::MemoryAllocator() {
        VmaAllocatorCreateInfo allocatorInfo{
                .physicalDevice = engine->physicalDevice,
                .device = engine->device,
                //.preferredLargeHeapBlockSize,
                //.pAllocationCallbacks,
                //.pDeviceMemoryCallbacks,
                .pHeapSizeLimit = nullptr,
                .pVulkanFunctions = nullptr,
                .instance = engine->instance,
        };
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