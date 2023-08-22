#ifndef SPHERE_VULKAN_CONTEXT_H
#define SPHERE_VULKAN_CONTEXT_H

#include "vulkan.h"
#include "glfw.h"
#include "vma.h"
#include "utils.h"
#include <stack>
#include <string>

namespace engine::renderer {

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

    class UploadContext {
    public:
        explicit UploadContext();
        ~UploadContext();

        void submit(std::function<void(VkCommandBuffer)> &&function);

    private:
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        VkFence fence;
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
        std::unique_ptr<UploadContext> uploadContext;

    private:
        DestroyQueue destroyQueue;

        void createInstance(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &requiredLayers);
        void createDebugMessenger();
        void createSurface();
        void pickPhysicalDevice(const std::vector<const char *> &requiredExtensions);
        void createDevice(const std::vector<const char *> &requiredExtensions);
        void createAllocator();
    };

    extern VulkanContext *context;

    VkCommandPool createCommandPool();
    std::vector<VkCommandBuffer> createCommandBuffers(const VkCommandPool &commandPool, size_t amount, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    VkFence createFence();

    namespace vk_create {
        VkImageCreateInfo image(const VkFormat &format, const VkExtent3D &extent, const VkImageUsageFlags &usage);
        VkImageViewCreateInfo imageView(const VkImage &image, const VkFormat &format, const VkImageAspectFlags &aspectMask);
        VkFramebufferCreateInfo framebuffer(const VkRenderPass &renderPass, const std::vector<VkImageView> &attachments, const VkExtent2D &extent);
    }
}

#endif //SPHERE_VULKAN_CONTEXT_H
