#ifndef SPHERE_ENGINE_HPP
#define SPHERE_ENGINE_HPP

#define GLFW_INCLUDE_VULKAN
#define VK_ENABLE_BETA_EXTENSIONS

#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>

#include <string>
#include <iostream>

#include <utils.hpp>

// the architectural design for the engine is to have a core with data that can be accessed by
// certain abstractions, such as a Texture.

// abstractions should emerge from usage and when there is too much code duplication.
// we shouldn't make arbitrary boundaries between pieces of code, because this can make it
// harder to rewrite into the right abstractions once they become evident.


namespace renderer {

    const std::string ENGINE_NAME = "Sphere";
    const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 0, 0);

    struct VulkanConfiguration {
        GLFWwindow *window;

        const std::string engineName;
        const std::string applicationName;
        const uint32_t engineVersion;
        const uint32_t applicationVersion;

        const bool debug;

        const std::vector<VkSurfaceFormatKHR> preferredSurfaceFormats;
        const std::vector<const char *> requiredInstanceExtensions;
        const std::vector<const char *> requiredInstanceLayers;
        const std::vector<const char *> requiredDeviceExtensions;
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

    struct PhysicalDeviceData {
        uint32_t minVertexInputBindingStrideAlignment;
        uint32_t maxVertexInputBindingStride;
    };

    /*
     * Common storage for all vulkan objects
     */
    struct VulkanData {
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice;
        PhysicalDeviceData physicalDeviceData; // cached physical device data
        QueueFamiliesData queueFamiliesData;
        SurfaceData surfaceData; // cached data
        VkDevice device; // logical device
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkSurfaceKHR surface;
    };

    /**
     * Data required for each frame that is rendered to
     */
    struct VulkanFrameData {
        VkImage swapchainImage;
        VkImageView swapchainImageView;
        VkFramebuffer swapchainFramebuffer;
        VkFence fence;
    };

    struct VulkanFrameSemaphores {
        VkSemaphore imageAcquiredSemaphore;
        VkSemaphore renderFinishedSemaphore;
    };

    class Engine {

    public:
        explicit Engine(const VulkanConfiguration &configuration);
        ~Engine();

        const VulkanConfiguration &configuration;
        VulkanData vulkanData;
    private:

        const std::vector<const char *> requiredDeviceExtensions{
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        void createInstance(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &requiredLayers);
        void createDebugMessenger();
        void destroyDebugMessenger();
        void createSurface();
        void pickPhysicalDevice(const std::vector<const char *> &requiredExtensions);
        void createDevice(const std::vector<const char *> &requiredExtensions);
    };

    void initializeEngine(const VulkanConfiguration &configuration);
    void destroyEngine();
    Engine &getEngine();
    const VulkanData &getVulkanData();
}

#endif //SPHERE_ENGINE_HPP
