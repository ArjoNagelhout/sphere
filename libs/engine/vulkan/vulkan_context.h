#ifndef SPHERE_VULKAN_CONTEXT_H
#define SPHERE_VULKAN_CONTEXT_H

#define GLFW_INCLUDE_VULKAN
#define VK_ENABLE_BETA_EXTENSIONS

#include "GLFW/glfw3.h"
#include <vulkan/vk_enum_string_helper.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

#include "vk_mem_alloc.h"

#pragma clang diagnostic pop

namespace engine {

    struct VulkanConfiguration {
        GLFWwindow *window;

        std::string engineName;
        std::string applicationName;
        uint32_t engineVersion;
        uint32_t applicationVersion;

        bool debug;

        std::vector<VkSurfaceFormatKHR> preferredSurfaceFormats;
        std::vector<const char *> requiredInstanceExtensions;
        std::vector<const char *> requiredInstanceLayers;
        std::vector<const char *> requiredDeviceExtensions;
    };

    struct QueueFamilyData {
        uint32_t index;
        VkQueueFamilyProperties properties;
    };

    /*
     * Data for each queue family (e.g. their index)
     */
    struct QueueFamiliesData {
        std::optional<QueueFamilyData> graphicsQueueFamilyData;
        std::optional<QueueFamilyData> presentQueueFamilyData;

        bool isComplete() {
            return graphicsQueueFamilyData.has_value() && presentQueueFamilyData.has_value();
        }
    };

    struct SurfaceData {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkPresentModeKHR> surfacePresentModes;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
    };

    class VulkanContext {

    public:
        explicit VulkanContext(VulkanConfiguration configuration);
        ~VulkanContext();

        VulkanConfiguration configuration;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice;
        QueueFamiliesData queueFamiliesData;
        SurfaceData surfaceData;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkSurfaceKHR surface;
        VmaAllocator allocator;

    private:
        void createInstance(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &requiredLayers);
        void createDebugMessenger();
        void destroyDebugMessenger();
        void createSurface();
        void pickPhysicalDevice(const std::vector<const char *> &requiredExtensions);
        void createDevice(const std::vector<const char *> &requiredExtensions);
        void createAllocator();
    };

    extern VulkanContext *context;
}

#endif //SPHERE_VULKAN_CONTEXT_H
