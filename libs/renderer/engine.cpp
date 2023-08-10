#include "engine.h"

#include <iostream>
#include <memory>
#include <cassert>

namespace renderer {

    std::unique_ptr<Engine> engine;

    Engine::Engine(const VulkanConfiguration &configuration) : configuration(configuration) {
        std::cout << "created engine" << std::endl;

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

    Engine::~Engine() {
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroyDebugMessenger();
        vkDestroyInstance(instance, nullptr);

        std::cout << "destroyed engine" << std::endl;
    }

    void initializeEngine(const VulkanConfiguration &configuration) {
        assert((!engine) && "Engine is already initialized. ");
        engine = std::make_unique<Engine>(configuration);
    }

    void destroyEngine() {
        engine.reset();
    }

    Engine &getEngine() {
        assert((engine) && "Engine should be initialized before calling this function, did you call initializeEngine()?");
        return *engine;
    }
}