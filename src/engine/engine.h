#ifndef SPHERE_ENGINE_H
#define SPHERE_ENGINE_H

#include <string>
#include <iostream>

#include <core/types.h>
#include <renderer/vulkan_context.h>

#include <renderer/utils.h>
#include <renderer/swapchain.h>
#include <renderer/render_pass.h>
#include <renderer/descriptor_set_builder.h>
#include <renderer/pipeline_builder.h>
#include <renderer/camera.h>
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/object.h"

namespace engine {

    const std::string ENGINE_NAME = "Sphere";
    const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 0, 0);

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

        // synchronization primitives
        VkFence inFlightFence;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;

        void initialize();
        void destroy() const;
    };

    /*
     * Engine is the main entry point that draws everything.
     */
    class Engine {

    public:
        explicit Engine(EngineConfiguration &engineConfiguration);
        ~Engine();

        std::unique_ptr<renderer::VulkanContext> context;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<DescriptorSetBuilder> descriptorSetBuilder;
        std::unique_ptr<PipelineBuilder> pipelineBuilder;
        std::unique_ptr<Camera> camera;
        std::unique_ptr<Scene> scene;

        VkCommandPool commandPool;
        bool framebufferResized = false;

        void render();

    private:
        const std::vector<const char *> requiredDeviceExtensions{
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t currentFrameIndex = 0;
        std::vector<FrameData> frames;

        const VkFormat depthImageFormat = VK_FORMAT_D16_UNORM;
        VkImage depthImage;
        VkImageView depthImageView;
        VmaAllocation depthImageAllocation;

        // drawing
        void drawFrame();
        void recordCommandBuffer(const FrameData &frameData, const VkFramebuffer &framebuffer);

        // to be refactored
        void createDepthImage();
    };

    extern Engine *engine;
}

#endif //SPHERE_ENGINE_H
