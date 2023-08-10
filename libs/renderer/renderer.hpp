#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "engine.hpp"
#include "swapchain.hpp"
#include "render_pass.hpp"
#include "graphics_pipeline.hpp"
#include "memory_allocator.hpp"

namespace renderer {

    struct CameraData {
        glm::mat4x4 MVP;
    };

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

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t currentFrameIndex = 0;
        std::vector<FrameData> frames;

        VkCommandPool commandPool;
        VkDescriptorPool descriptorPool;

        // buffers

        CameraData cameraData;
        VkBuffer cameraDataBuffer;
        VmaAllocation cameraDataBufferAllocation;

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

#endif //SPHERE_RENDERER_HPP
