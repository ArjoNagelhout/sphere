#ifndef SPHERE_SWAPCHAIN_HPP
#define SPHERE_SWAPCHAIN_HPP

#include "engine.hpp"

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

        VkSurfaceFormatKHR surfaceFormat;
        VkExtent2D extent;

    private:
        Engine &engine;
        VkSwapchainKHR swapchain;

        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
    };
}

#endif //SPHERE_SWAPCHAIN_HPP