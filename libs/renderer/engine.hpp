#ifndef SPHERE_ENGINE_HPP
#define SPHERE_ENGINE_HPP

#include <vulkan/vulkan.h>
#include <string>

namespace renderer {

    struct VulkanData {
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device; // logical device
        VkQueue graphicsQueue;
        VkQueue presentQueue;
    };

    /**
     * Data for rendering to a surface
     */
    struct VulkanWindowData {
        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        VkSurfaceFormatKHR surfaceFormat;
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
        explicit Engine(const std::string &test);
        ~Engine();
        void doSomething();

    private:
        VulkanData vulkanData;
        VulkanWindowData vulkanWindowData;
    };

    void initializeEngine();
    void destroyEngine();
    Engine &getEngine();
    VulkanData getVulkanData();
    VulkanWindowData getVulkanWindowData();
}

#endif //SPHERE_ENGINE_HPP
