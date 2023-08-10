#ifndef SPHERE_SWAPCHAIN_H
#define SPHERE_SWAPCHAIN_H

#include "engine.h"

namespace renderer {

    const std::vector<VkSurfaceFormatKHR> preferredSurfaceFormats{
            {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
    };

    /*
     * A swapchain provides the ability to present rendering results to a surface.
     *
     * A swapchain is an abstraction for an array of presentable images that are associated with a surface.
     * The presentable images are represented by VkImage objects created by the platform.
     *
     * One image (which can be an array image for multiview/stereoscopic-3D surfaces) is displayed at a time,
     * but multiple images can be queued for presentation.
     *
     * An application renders to the image, and then queues the image for presentation to the surface.
     *
     * Only one active swapchain can be bound to a surface.
     *
     * The swapchain should be recreated when the window is resized. For performance, we can use the old swapchain as
     * a basis for the new swapchain using creatInfo.oldSwapchain.
     *
     * The application acquires VkImages from the *presentation engine*, which is the platform's compositor or display engine.
     *
     * use presentable image only after vkAcquireNextImageKHR, and before vkQueuePresentKHR
     */
    class Swapchain{

    public:
        explicit Swapchain(const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats);
        ~Swapchain();

        VkSwapchainKHR swapchain;
        VkSurfaceFormatKHR surfaceFormat;
        VkExtent2D extent;
        std::vector<VkFramebuffer> framebuffers;

        void createFramebuffers(const VkRenderPass &renderPass);

    private:
        Engine &engine;

        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;

        void createSwapchain(const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats);
        void createImageViews();
    };
}

#endif //SPHERE_SWAPCHAIN_H