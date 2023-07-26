#ifndef SPHERE_SWAPCHAIN_HPP
#define SPHERE_SWAPCHAIN_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "utils.hpp"
#include "device.hpp"

#include <stdexcept>
#include <string>

namespace renderer {

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

        static void createSwapchain(Device &device, VkSwapchainKHR &swapchain, const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats = {}) {

            SurfaceData surfaceData = device.getSurfaceData();
            VkSurfaceFormatKHR surfaceFormat = pickSwapchainSurfaceFormat(surfaceData.surfaceFormats, preferredSurfaceFormats);

            VkSwapchainCreateInfoKHR swapchainCreateInfo{};
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.surface = device.getSurface();
            swapchainCreateInfo.presentMode = pickSwapchainPresentMode(surfaceData.surfacePresentModes);
            swapchainCreateInfo.imageFormat = surfaceFormat.format;
            swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;

            VkResult result = vkCreateSwapchainKHR(device.getDevice(), &swapchainCreateInfo, nullptr, &swapchain);
            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create swapchain: ") + string_VkResult(result));
            }
        }

    };

}

#endif //SPHERE_SWAPCHAIN_HPP
