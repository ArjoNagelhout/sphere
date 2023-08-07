#ifndef SPHERE_ENGINE_HPP
#define SPHERE_ENGINE_HPP

#include <vulkan/vulkan.h>

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
}

#endif //SPHERE_ENGINE_HPP
