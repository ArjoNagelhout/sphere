#ifndef SPHERE_SWAPCHAIN_HPP
#define SPHERE_SWAPCHAIN_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "utils.hpp"
#include "device.hpp"

#include <stdexcept>
#include <string>

namespace renderer {

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
    class Swapchain {

    public:
        explicit Swapchain(Device &device) : device(device) {
            createSwapchain(device, swapchain, preferredSurfaceFormats);
        }

        ~Swapchain() {
            vkDestroySwapchainKHR(device.getDevice(), swapchain, nullptr);
        }

    private:
        Device &device;
        VkSwapchainKHR swapchain;

        const std::vector<VkSurfaceFormatKHR> preferredSurfaceFormats{
                {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
        };

        static VkPresentModeKHR pickSwapchainPresentMode(std::vector<VkPresentModeKHR> surfacePresentModes) {
            return VK_PRESENT_MODE_FIFO_KHR; // high performance, default
        }

        /*
         * Sorts surface formats based on the supplied preferred surface formats,
         * first item in list will be chosen first
         */
        static VkSurfaceFormatKHR pickSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats,
                                                             const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats = {}) {
            // iterate over preferred surface formats and see if they are supported, if so pick that one
            for (const VkSurfaceFormatKHR &preferredSurfaceFormat : preferredSurfaceFormats) {
                if (*std::find_if(surfaceFormats.begin(),
                             surfaceFormats.end(),
                             [&preferredSurfaceFormat](const VkSurfaceFormatKHR surfaceFormat) -> bool {
                    return preferredSurfaceFormat == surfaceFormat;
                }) != *surfaceFormats.end()) {
                    std::cout << "found preferred surface format: " << string_VkFormat(preferredSurfaceFormat.format)
                        << ", color space: " << string_VkColorSpaceKHR(preferredSurfaceFormat.colorSpace)
                        << std::endl;

                    return preferredSurfaceFormat;
                }
            }

            // otherwise return first surface format in list by default
            return surfaceFormats[0];
        }

        static VkExtent2D pickSwapchainExtent() {
            return {0, 0};
        }

        static void createSwapchain(Device &device, VkSwapchainKHR &swapchain, const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats = {}) {

            SurfaceData surfaceData = device.getSurfaceData();
            VkSurfaceFormatKHR surfaceFormat = pickSwapchainSurfaceFormat(surfaceData.surfaceFormats, preferredSurfaceFormats);
            VkExtent2D extent = pickSwapchainExtent();

            VkSwapchainCreateInfoKHR swapchainCreateInfo{};
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.surface = device.getSurface();
            //swapchainCreateInfo.minImageCount = surfaceData.surfaceCapabilities.minImageCount;
            swapchainCreateInfo.presentMode = pickSwapchainPresentMode(surfaceData.surfacePresentModes);
            swapchainCreateInfo.imageFormat = surfaceFormat.format;
            swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
            swapchainCreateInfo.imageExtent = extent;
            swapchainCreateInfo.imageArrayLayers = 1; // for stereoscopic rendering should be more than 1
            swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // listed in surfaceCapabilities.supportedUsageFlags, but VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT is guaranteed to always exist

            // determine if the graphics queue family and the present queue family share the same index
            QueueFamiliesData queueFamiliesData = device.getQueueFamiliesData();
            uint32_t queueFamilyIndices[]{queueFamiliesData.graphicsQueueFamilyData->index, queueFamiliesData.presentQueueFamilyData->index};

            if (queueFamilyIndices[0] == queueFamilyIndices[1]) {
                swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchainCreateInfo.queueFamilyIndexCount = 2;
                swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
            } else {
                // An image is owned by one queue family at a time and ownership must be explicitly transferred before
                // using it in another queue family. This option offers the best performance.
                swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                swapchainCreateInfo.queueFamilyIndexCount = 0; // optional
                swapchainCreateInfo.pQueueFamilyIndices = nullptr; // optional
            }

            VkResult result = vkCreateSwapchainKHR(device.getDevice(), &swapchainCreateInfo, nullptr, &swapchain);
            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create swapchain: ") + string_VkResult(result));
            }
        }

    };

}

#endif //SPHERE_SWAPCHAIN_HPP
