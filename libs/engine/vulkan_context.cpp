#include "vulkan_context.h"

#include <iostream>
#include <memory>
#include <cassert>

namespace engine {

    std::unique_ptr<VulkanContext> context;

    VulkanContext::VulkanContext(const VulkanConfiguration &configuration) : configuration(configuration) {
        std::cout << "created vulkan context" << std::endl;

        std::vector<const char *> allRequiredInstanceExtensions{configuration.requiredInstanceExtensions.begin(), configuration.requiredInstanceExtensions.end()};
        std::vector<const char *> allRequiredInstanceLayers{configuration.requiredInstanceLayers.begin(), configuration.requiredInstanceLayers.end()};

        if (configuration.debug) {
            allRequiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            allRequiredInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        createInstance(allRequiredInstanceExtensions, allRequiredInstanceLayers);
        createDebugMessenger();
        createSurface();
        pickPhysicalDevice(requiredDeviceExtensions);
        createDevice(requiredDeviceExtensions);
    }

    VulkanContext::~VulkanContext() {
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroyDebugMessenger();
        vkDestroyInstance(instance, nullptr);

        std::cout << "destroyed vulkan context" << std::endl;
    }

    void initializeContext(const VulkanConfiguration &configuration) {
        assert((!context) && "Vulkan context is already initialized. ");
        context = std::make_unique<VulkanContext>(configuration);
    }

    void destroyContext() {
        context.reset();
    }

    VulkanContext &getContext() {
        assert((context) && "Vulkan context should be initialized before calling this function, did you call initializeContext()?");
        return *context;
    }
}