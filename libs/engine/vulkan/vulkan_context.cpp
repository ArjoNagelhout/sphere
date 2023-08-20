#define VMA_IMPLEMENTATION
#include "vulkan_context.h"
#include "utils.h"

#include <iostream>
#include <map>

namespace engine {

    VulkanContext *context;

    VulkanContext::VulkanContext(VulkanConfiguration configuration) {

        // this can be refactored in the future to allow multiple devices, but since we're
        // writing an application meant to be run on mobile platforms, this is not required.
        assert((context == nullptr) && "Only one vulkan context can exist at one time");
        context = this;

        std::vector<const char *> allRequiredInstanceExtensions{configuration.requiredInstanceExtensions.begin(),
                                                                configuration.requiredInstanceExtensions.end()};
        std::vector<const char *> allRequiredInstanceLayers{configuration.requiredInstanceLayers.begin(),
                                                            configuration.requiredInstanceLayers.end()};

        if (configuration.debug) {
            allRequiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            allRequiredInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        createInstance(allRequiredInstanceExtensions, allRequiredInstanceLayers);
        createDebugMessenger();
        createSurface();
        pickPhysicalDevice(configuration.requiredDeviceExtensions);
        createDevice(configuration.requiredDeviceExtensions);
        createAllocator();
    }

    VulkanContext::~VulkanContext() {

        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroyDebugMessenger();
        vkDestroyInstance(instance, nullptr);

        context = nullptr;
    }

    void VulkanContext::createAllocator() {
        VmaAllocatorCreateInfo allocatorInfo{
                .physicalDevice = physicalDevice,
                .device = device,
                //.preferredLargeHeapBlockSize,
                //.pAllocationCallbacks,
                //.pDeviceMemoryCallbacks,
                .pHeapSizeLimit = nullptr,
                .pVulkanFunctions = nullptr,
                .instance = instance,
        };
        checkResult(vmaCreateAllocator(&allocatorInfo, &allocator));
        std::cout << "created memory allocator" << std::endl;
    }
}