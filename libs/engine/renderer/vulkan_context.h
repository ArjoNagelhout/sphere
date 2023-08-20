#ifndef SPHERE_VULKAN_CONTEXT_H
#define SPHERE_VULKAN_CONTEXT_H

#include "includes.h"

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

        void immediateSubmit(std::function<void(VkCommandBuffer)>&& function);

    private:
        VkCommandPool immediateSubmitCommandPool;
        VkCommandBuffer immediateSubmitCommandBuffer;
        VkFence immediateSubmitFence;

        void createInstance(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &requiredLayers);
        void createDebugMessenger();
        void destroyDebugMessenger();
        void createSurface();
        void pickPhysicalDevice(const std::vector<const char *> &requiredExtensions);
        void createDevice(const std::vector<const char *> &requiredExtensions);
        void createAllocator();
        void createImmediateSubmitContext();
    };

    extern VulkanContext *context;

    VkCommandPool createCommandPool();
    std::vector<VkCommandBuffer> createCommandBuffers(const VkCommandPool &commandPool, size_t amount, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    VkFence createFence();
}

#endif //SPHERE_VULKAN_CONTEXT_H
