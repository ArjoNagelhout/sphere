#define VMA_IMPLEMENTATION

#include "vulkan_context.h"
#include "utils.h"

#include <iostream>
#include <map>

namespace engine::renderer {

    VulkanContext *context;

    VulkanContext::VulkanContext(VulkanConfiguration configuration) : configuration(configuration) {

        // this can be refactored in the future to allow multiple devices, but since we're
        // writing an application meant to be run on mobile platforms, this is not required.
        assert((context == nullptr) && "Only one vulkan context can exist at one time");
        context = this;

        std::vector<const char *> allRequiredInstanceExtensions{configuration.requiredInstanceExtensions.begin(),
                                                                configuration.requiredInstanceExtensions.end()};
        std::vector<const char *> allRequiredInstanceLayers{configuration.requiredInstanceLayers.begin(),
                                                            configuration.requiredInstanceLayers.end()};

        if (configuration.debug) {
            allRequiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            allRequiredInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        createInstance(allRequiredInstanceExtensions, allRequiredInstanceLayers);
        createDebugMessenger();
        createSurface();
        pickPhysicalDevice(configuration.requiredDeviceExtensions);
        createDevice(configuration.requiredDeviceExtensions);
        createAllocator();
        uploadContext = std::make_unique<UploadContext>();
        destroyQueue.push([&]() { uploadContext.reset(); });
    }

    VulkanContext::~VulkanContext() {
        destroyQueue.flush();
        context = nullptr;
    }

    void VulkanContext::createAllocator() {
        VmaAllocatorCreateInfo allocatorInfo{
                .physicalDevice = physicalDevice,
                .device = device,
                //.preferredLargeHeapBlockSize,
                //.pAllocationCallbacks,
                //.pDeviceMemoryCallbacks,
                .pHeapSizeLimit = nullptr,
                .pVulkanFunctions = nullptr,
                .instance = instance,
        };

        checkResult(vmaCreateAllocator(&allocatorInfo, &allocator));
        destroyQueue.push([&]() {
            vmaDestroyAllocator(allocator);
        });

        std::cout << "created memory allocator" << std::endl;
    }

    UploadContext::UploadContext() {
        commandPool = createCommandPool();
        commandBuffer = createCommandBuffers(commandPool, 1)[0];
        fence = createFence();
    }

    UploadContext::~UploadContext() {
        vkDestroyFence(context->device, fence, nullptr);
        vkDestroyCommandPool(context->device, commandPool, nullptr);

//        std::cout << "destroyed upload context" << std::endl;
    }

    void UploadContext::submit(std::function<void(VkCommandBuffer)> &&function) {
        const VkCommandBuffer &cmd = commandBuffer;

        vkDeviceWaitIdle(context->device);

        // upload the image to the read only shader layout
        checkResult(vkResetCommandPool(context->device, commandPool, 0));
        VkCommandBufferBeginInfo beginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        checkResult(vkBeginCommandBuffer(cmd, &beginInfo));

        function(cmd);

        VkSubmitInfo submitInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = nullptr,
                .commandBufferCount = 1,
                .pCommandBuffers = &cmd,
        };
        checkResult(vkEndCommandBuffer(cmd));
        checkResult(vkQueueSubmit(context->graphicsQueue, 1, &submitInfo, fence));

        vkWaitForFences(context->device, 1, &fence, true, UINT64_MAX);
        vkResetFences(context->device, 1, &fence);
    }

    VkCommandPool createCommandPool() {
        VkCommandPool commandPool;
        QueueFamiliesData data = context->queueFamiliesData;

        VkCommandPoolCreateInfo info{
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = data.graphicsQueueFamilyData->index,
        };
        checkResult(vkCreateCommandPool(context->device, &info, nullptr, &commandPool));
        return commandPool;
    }

    std::vector<VkCommandBuffer>
    createCommandBuffers(const VkCommandPool &commandPool, size_t amount, VkCommandBufferLevel level) {
        std::vector<VkCommandBuffer> commandBuffers(amount);

        VkCommandBufferAllocateInfo allocateInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = commandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
        };
        checkResult(vkAllocateCommandBuffers(context->device, &allocateInfo, commandBuffers.data()));
        return commandBuffers;
    }

    VkFence createFence() {
        VkFence fence;
        VkFenceCreateInfo fenceInfo{
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0
        };
        checkResult(vkCreateFence(context->device, &fenceInfo, nullptr, &fence));
        return fence;
    }
}