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

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = engine->surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1; // for stereoscopic rendering should be more than 1
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // listed in surfaceCapabilities.supportedUsageFlags, but VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT is guaranteed to always exist

        // determine if the graphics queue family and the present queue family share the same index
        QueueFamiliesData queueFamiliesData = engine->queueFamiliesData;
        uint32_t queueFamilyIndices[]{queueFamiliesData.graphicsQueueFamilyData->index,
                                      queueFamiliesData.presentQueueFamilyData->index};

        if (queueFamilyIndices[0] == queueFamilyIndices[1]) {
            // This option offers the best performance.
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0; // optional
            swapchainCreateInfo.pQueueFamilyIndices = nullptr; // optional
        } else {
            // an image is owned by one queue family at a time and ownership must be explicitly transferred before
            // using it in another queue family.
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }

        swapchainCreateInfo.preTransform = surfaceData.surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ignores the alpha channel when compositing the window with other surfaces on certain window systems.
        swapchainCreateInfo.presentMode = pickSwapchainPresentMode(surfaceData.surfacePresentModes);
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

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

            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            // If you were working on a stereographic 3D application, then you would create a swap chain
            // with multiple layers. You could then create multiple image views for each image representing
            // the views for the left and right eyes by accessing different layers.

            checkResult(vkCreateImageView(engine->device, &createInfo, nullptr, &imageViews[i]));
        }
    }

    void Swapchain::createFramebuffers(const VkRenderPass &renderPass) {
        framebuffers.resize(images.size());

        for (size_t i = 0; i < imageViews.size(); i++) {
            std::vector<VkImageView> attachments{
                    imageViews[i]
            };

            VkFramebufferCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.renderPass = renderPass;
            createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            createInfo.pAttachments = attachments.data();
            createInfo.width = extent.width;
            createInfo.height = extent.height;
            createInfo.layers = 1;

            checkResult(vkCreateFramebuffer(engine->device, &createInfo, nullptr, &framebuffers[i]));
        }
        std::cout << "created frame buffers" << std::endl;
    }
}