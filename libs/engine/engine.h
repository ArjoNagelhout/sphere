#ifndef SPHERE_ENGINE_H
#define SPHERE_ENGINE_H

#define GLFW_INCLUDE_VULKAN
#define VK_ENABLE_BETA_EXTENSIONS

#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>

#include <string>
#include <iostream>

#include "types.h"
#include "utils.h"
#include "swapchain.h"
#include "render_pass.h"
#include "graphics_pipeline.h"
#include "memory_allocator.h"
#include "camera.h"

namespace engine {

    const std::string ENGINE_NAME = "Sphere";
    const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 0, 0);

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

    struct PhysicalDeviceData {
        uint32_t minVertexInputBindingStrideAlignment;
        uint32_t maxVertexInputBindingStride;
    };

    struct EngineConfiguration {
        GLFWwindow *window;

        bool debug;

        const std::string applicationName;
        const uint32_t applicationVersion;
    };

    /*
     * Data for each frame
     */
    struct FrameData {
        VkCommandBuffer commandBuffer;
        VkDescriptorSet descriptorSet;

        // synchronization primitives
        VkFence inFlightFence;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;

        void initialize();
        void destroy() const;
        void updateDescriptorSet(VkBuffer &buffer, VkDescriptorType descriptorType) const;
    };

    /*
     * Engine is the main entry point that draws everything.
     *
     * todo: refactor VulkanContext into this
     *
     * The current step is going to be the ability to render multiple objects and somehow represent those in a scene
     */
    class Engine {

    public:
        explicit Engine(EngineConfiguration &engineConfiguration);
        ~Engine();

        void render();

        VulkanConfiguration configuration;

        // vulkan data
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

        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<MemoryAllocator> allocator;
        std::unique_ptr<Camera> camera;

    private:

        const std::vector<const char *> requiredDeviceExtensions{
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t currentFrameIndex = 0;
        std::vector<FrameData> frames;

        VkCommandPool commandPool;
        VkDescriptorPool descriptorPool;

        // buffers
        std::vector<VertexAttributes> vertices{
                {{-0.5f, -0.5f, 0}},
                {{0.5f,  -0.5f, 0}},
                {{0.5f,  0.5f,  0}},
                {{-0.5f, 0.5f,  0}}
        };
        VkBuffer vertexBuffer;
        VmaAllocation vertexBufferAllocation;

        std::vector<uint32_t> indices{
                0, 1, 2, 2, 3, 0
        };
        VkBuffer indexBuffer;
        VmaAllocation indexBufferAllocation;

        VkImage depthImage;
        VkImageView depthImageView;
        VmaAllocation depthImageAllocation;

        // vulkan setup
        void createInstance(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &requiredLayers);
        void createDebugMessenger();
        void destroyDebugMessenger();
        void createSurface();
        void pickPhysicalDevice(const std::vector<const char *> &requiredExtensions);
        void createDevice(const std::vector<const char *> &requiredExtensions);

        // drawing
        void createCommandPool();
        void createDescriptorPool(const VkDescriptorType &descriptorType);
        std::vector<VkCommandBuffer> allocateCommandBuffers();
        std::vector<VkDescriptorSet> allocateDescriptorSets(VkDescriptorSetLayout &descriptorSetLayout);
        void drawFrame(FrameData frameData);
        void recordCommandBuffer(FrameData frameData, VkFramebuffer framebuffer);
    };

    extern Engine *engine;
}

#endif //SPHERE_ENGINE_H
