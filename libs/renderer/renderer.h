#ifndef SPHERE_RENDERER_H
#define SPHERE_RENDERER_H

#include "engine.h"
#include "swapchain.h"
#include "render_pass.h"
#include "graphics_pipeline.h"
#include "memory_allocator.h"
#include "camera.h"

namespace renderer {

    struct RendererConfiguration {
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
        VkDescriptorSet descriptorSet;

        // synchronization primitives
        VkFence inFlightFence;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;

        void initialize();
        void destroy() const;
        void updateDescriptorSet(VkBuffer &buffer, VkDescriptorType descriptorType) const;
    };

    class Renderer {

    public:
        explicit Renderer(RendererConfiguration &configuration);
        ~Renderer();

        void render();

    private:
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;
        std::unique_ptr<MemoryAllocator> memoryAllocator;
        std::unique_ptr<Camera> camera;

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t currentFrameIndex = 0;
        std::vector<FrameData> frames;

        VkCommandPool commandPool;
        VkDescriptorPool descriptorPool;

        // buffers
        std::vector<VertexAttributes> vertices{
                {{-0.5f, -0.5f, 0}},
                {{0.5f,  -0.5f, 0}},
                {{0.5f,  0.5f,  0}},
                {{-0.5f, 0.5f,  0}}
        };
        VkBuffer vertexBuffer;
        VmaAllocation vertexBufferAllocation;

        std::vector<uint32_t> indices{
                0, 1, 2, 2, 3, 0
        };
        VkBuffer indexBuffer;
        VmaAllocation indexBufferAllocation;

        void createCommandPool();
        void createDescriptorPool(const VkDescriptorType &descriptorType);
        std::vector<VkCommandBuffer> allocateCommandBuffers();
        std::vector<VkDescriptorSet> allocateDescriptorSets(VkDescriptorSetLayout &descriptorSetLayout);
        void drawFrame(FrameData frameData);
        void recordCommandBuffer(FrameData frameData, VkFramebuffer framebuffer);
    };
}

#endif //SPHERE_RENDERER_H
