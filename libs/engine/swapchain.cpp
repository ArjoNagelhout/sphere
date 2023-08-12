#include "engine.h"
#include "swapchain.h"

namespace engine {

    static VkPresentModeKHR pickSwapchainPresentMode(std::vector<VkPresentModeKHR> &surfacePresentModes) {
        return VK_PRESENT_MODE_FIFO_KHR; // high performance, default
    }

    /*
     * Sorts surface formats based on the supplied preferred surface formats,
     * first item in list will be chosen first
     */
    static VkSurfaceFormatKHR pickSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats,
                                                         const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats = {}) {
        // iterate over preferred surface formats and see if they are supported, if so pick that one
        for (const VkSurfaceFormatKHR &preferredSurfaceFormat: preferredSurfaceFormats) {
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

    static VkExtent2D pickSwapchainExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
        if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            // vulkan says: extent should match window extent. but for some window managers
            // this is not required, this is indicated with setting the width and height extent
            // to the max value of uint32_t
            return surfaceCapabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width,
                                            surfaceCapabilities.minImageExtent.width,
                                            surfaceCapabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height,
                                             surfaceCapabilities.minImageExtent.height,
                                             surfaceCapabilities.maxImageExtent.height);
            return actualExtent;
        }
    }

    Swapchain::Swapchain(const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats) {
        createSwapchain(preferredSurfaceFormats);
        createImageViews();
    }

    Swapchain::~Swapchain() {
        for (auto const &framebuffer: framebuffers) {
            vkDestroyFramebuffer(engine->device, framebuffer, nullptr);
        }

        for (auto const &imageView: imageViews) {
            vkDestroyImageView(engine->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(engine->device, swapchain, nullptr);
    }

    void Swapchain::createSwapchain(const std::vector<VkSurfaceFormatKHR> &preferredSurfaceFormats) {
        SurfaceData surfaceData = engine->surfaceData;
        surfaceFormat = pickSwapchainSurfaceFormat(surfaceData.surfaceFormats, preferredSurfaceFormats);
        extent = pickSwapchainExtent(engine->configuration.window, surfaceData.surfaceCapabilities);

        // from vulkan-tutorial.com
        // Sticking to this minimum means that we may sometimes have to wait on the driver to complete internal
        // operations before we can acquire another image to render to. Therefore, it is recommended to request
        // at least one more image than the minimum:
        uint32_t imageCount =
                surfaceData.surfaceCapabilities.minImageCount + 1; // minImageCount is guaranteed to be at least 1

        // limit image count
        // if maxImageCount is 0, that means there is no hard limit for the amount of images.
        if (surfaceData.surfaceCapabilities.maxImageCount > 0 &&
            imageCount > surfaceData.surfaceCapabilities.maxImageCount) {
            imageCount = surfaceData.surfaceCapabilities.maxImageCount;
        }

        // determine if the graphics queue family and the present queue family share the same index
        QueueFamiliesData queueFamiliesData = engine->queueFamiliesData;
        uint32_t queueFamilyIndices[]{queueFamiliesData.graphicsQueueFamilyData->index,
                                      queueFamiliesData.presentQueueFamilyData->index};
        bool oneFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

        VkSwapchainCreateInfoKHR swapchainCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = engine->surface,
                .minImageCount = imageCount,
                .imageFormat = surfaceFormat.format,
                .imageColorSpace = surfaceFormat.colorSpace,
                .imageExtent = extent,
                .imageArrayLayers = 1, // for stereoscopic rendering should be more than 1
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // listed in surfaceCapabilities.supportedUsageFlags, but VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT is guaranteed to always exist
                .imageSharingMode = oneFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
                .queueFamilyIndexCount = static_cast<uint32_t>(oneFamily ? 0 : 2),
                .pQueueFamilyIndices = oneFamily ? nullptr : queueFamilyIndices,
                .preTransform = surfaceData.surfaceCapabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // ignores the alpha channel when compositing the window with other surfaces on certain window systems.
                .presentMode = pickSwapchainPresentMode(surfaceData.surfacePresentModes),
                .clipped = VK_TRUE,
                .oldSwapchain = VK_NULL_HANDLE,
        };

        checkResult(vkCreateSwapchainKHR(engine->device, &swapchainCreateInfo, nullptr, &swapchain));
        std::cout << "created swapchain" << std::endl;

        // get the swapchain images of the created swap chain. This is similar to the VkPhysicalDevice objects, which are "owned" by the VkInstance.
        // in order to perform any rendering operations, create a VkImageView from a VkImage.
        uint32_t swapchainImagesCount;
        vkGetSwapchainImagesKHR(engine->device, swapchain, &swapchainImagesCount, nullptr);
        images.resize(swapchainImagesCount);
        vkGetSwapchainImagesKHR(engine->device, swapchain, &swapchainImagesCount, images.data());
    }

    void Swapchain::createImageViews() {
        imageViews.resize(images.size());

        for (size_t i = 0; i < images.size(); i++) {
            // If you were working on a stereographic 3D application, then you would create a swap chain
            // with multiple layers. You could then create multiple image views for each image representing
            // the views for the left and right eyes by accessing different layers.
            VkImageViewCreateInfo viewInfo = vk_create::imageView(images[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
            checkResult(vkCreateImageView(engine->device, &viewInfo, nullptr, &imageViews[i]));
        }
    }

    void Swapchain::createFramebuffers(const VkRenderPass &renderPass, const VkImageView &depthImageView) {
        framebuffers.resize(images.size());

        for (size_t i = 0; i < imageViews.size(); i++) {
            std::vector<VkImageView> attachments{
                    imageViews[i],
                    depthImageView
            };
            VkFramebufferCreateInfo framebufferInfo = vk_create::framebuffer(renderPass, attachments, extent);
            checkResult(vkCreateFramebuffer(engine->device, &framebufferInfo, nullptr, &framebuffers[i]));
        }
        std::cout << "created frame buffers" << std::endl;
    }
}