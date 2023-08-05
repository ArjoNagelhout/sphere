#ifndef SPHERE_RENDERER_HPP
#define SPHERE_RENDERER_HPP

#include "device.hpp"
#include "swapchain.hpp"
#include "graphics_pipeline.hpp"
#include "memory_allocator.hpp"
#include "vertex_data.hpp"

#include <iostream>
#include <memory>
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <glm/gtc/matrix_transform.hpp>

namespace renderer {

    struct CameraData {
        glm::mat4 MVP;
    };

    /*
     * The Renderer class ties together the different Vulkan objects and
     * is the main entry point for getting things onto the screen.
     */
    class Renderer {

    public:
        explicit Renderer(const std::string &applicationName, bool debug) {
            window = std::make_unique<Window>(applicationName, 600, 300, 200, 100);
            device = std::make_unique<Device>(*window, debug);
            swapchain = std::make_unique<Swapchain>(*window, *device);
            renderPass = std::make_unique<RenderPass>(*device, *swapchain);
            swapchain->createSwapchainFramebuffers(renderPass->getRenderPass());
            graphicsPipeline = std::make_unique<GraphicsPipeline>(*device, *swapchain, *renderPass);
            memoryAllocator = std::make_unique<MemoryAllocator>(*device);

            createCommandPool(*device, commandPool);
            createCommandBuffers(device->getDevice(),
                                 commandPool,
                                 MAX_FRAMES_IN_FLIGHT,
                                 commandBuffers);
            createDescriptorPool(device->getDevice(), graphicsPipeline->descriptorType, descriptorPool,
                                 MAX_FRAMES_IN_FLIGHT);

            updateCameraData(cameraData, swapchain->getSwapchainExtent(), glm::vec3(7, 3, 7));
            createBuffer<CameraData>(memoryAllocator->getAllocator(),
                                     cameraDataBuffer,
                                     cameraDataBufferAllocation,
                                     cameraData,
                                     sizeof(cameraData),
                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

            createDescriptorSets(device->getDevice(),
                                 descriptorPool,
                                 MAX_FRAMES_IN_FLIGHT,
                                 graphicsPipeline->getDescriptorSetLayout(),
                                 graphicsPipeline->descriptorType,
                                 cameraDataBuffer,
                                 descriptorSets);
            createSynchronizationPrimitives(device->getDevice(),
                                            imageAvailableSemaphores,
                                            renderFinishedSemaphores,
                                            inFlightFences,
                                            MAX_FRAMES_IN_FLIGHT);
            std::string path = "/Users/arjonagelhout/Documents/ShapeReality/sphere/external/tinyobjloader/models/map-bump.obj";
            //"/Users/arjonagelhout/Documents/ShapeReality/sphere/external/tinyobjloader/models/catmark_torus_creases0.obj";
            // "/Users/arjonagelhout/Documents/ShapeReality/sphere/external/tinyobjloader/models/cube.obj";
            //"/Users/arjonagelhout/Documents/ShapeReality/sphere/external/tinyobjloader/models/cornell_box_multimaterial.obj";
            loadObj(path,
                    vertices,
                    indices);
            createBuffer<VertexData>(memoryAllocator->getAllocator(),
                                     vertexBuffer,
                                     vertexBufferAllocation,
                                     *vertices.data(),
                                     vertices.size() * sizeof(vertices[0]),
                                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            createBuffer<uint32_t>(memoryAllocator->getAllocator(),
                                   indexBuffer,
                                   indexBufferAllocation,
                                   *indices.data(),
                                   indices.size() * sizeof(indices[0]),
                                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        }

        ~Renderer() {
            vmaDestroyBuffer(memoryAllocator->getAllocator(), vertexBuffer, vertexBufferAllocation);
            vmaDestroyBuffer(memoryAllocator->getAllocator(), indexBuffer, indexBufferAllocation);
            vmaDestroyBuffer(memoryAllocator->getAllocator(), cameraDataBuffer, cameraDataBufferAllocation);

            // destroy synchronization primitives
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(device->getDevice(), imageAvailableSemaphores[i], nullptr);
                vkDestroySemaphore(device->getDevice(), renderFinishedSemaphores[i], nullptr);
                vkDestroyFence(device->getDevice(), inFlightFences[i], nullptr);
            }

            vkDestroyDescriptorPool(device->getDevice(), descriptorPool, nullptr);
            vkDestroyCommandPool(device->getDevice(), commandPool, nullptr);
        }

        void run() {
            GLFWwindow *glfwWindow = window->getWindow();

            while (!glfwWindowShouldClose(glfwWindow)) {
                glfwPollEvents();
                updateCamera();
                drawFrame(currentFrame,
                          MAX_FRAMES_IN_FLIGHT,
                          *device,
                          *swapchain,
                          *graphicsPipeline,
                          *renderPass,
                          commandBuffers,
                          descriptorSets,
                          imageAvailableSemaphores,
                          renderFinishedSemaphores,
                          inFlightFences,
                          vertexBuffer,
                          indexBuffer,
                          indices);
            }
            vkDeviceWaitIdle(device->getDevice());
        }

    private:
        std::unique_ptr<Window> window;
        std::unique_ptr<Device> device;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;
        std::unique_ptr<MemoryAllocator> memoryAllocator;

        const int MAX_FRAMES_IN_FLIGHT = 2;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        uint32_t currentFrame = 0;

        uint32_t time = 0;

        CameraData cameraData;

        std::vector<VertexData> vertices{
                {{-0.5f, -0.5f, 0}},
                {{0.5f,  -0.5f, 0}},
                {{0.5f,  0.5f,  0}},
                {{-0.5f, 0.5f,  0}}
        };

        std::vector<uint32_t> indices{
                0, 1, 2, 2, 3, 0
        };

        VkBuffer vertexBuffer;
        VmaAllocation vertexBufferAllocation;

        VkBuffer indexBuffer;
        VmaAllocation indexBufferAllocation;

        VkBuffer cameraDataBuffer;
        VmaAllocation cameraDataBufferAllocation;

        float getPos(const float &speed, const float &amplitude, const float timeOffset, const uint32_t &time) {
            return amplitude * std::sin((float)time * speed + timeOffset);
        }

        void updateCamera() {

            // using a sine wave
            float speed = 0.05f;
            float amplitude = 1.0f;


            time++;

            updateCameraData(cameraData,
                             swapchain->getSwapchainExtent(),
                             glm::vec3(
                                     7,
                                     getPos(0.05f, 10.0f, 1.57f, time),
                                      getPos(0.05f, 10.0f, 0.0f, time)));
            updateBuffer<CameraData>(memoryAllocator->getAllocator(),
                                     cameraDataBuffer,
                                     cameraDataBufferAllocation,
                                     cameraData,
                                     sizeof(cameraData));


        }

        static void updateCameraData(CameraData &cameraData, const VkExtent2D &extent, glm::vec3 newPosition) {

            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            glm::mat4 Projection = glm::perspective(glm::radians(60.0f), (float) extent.width / (float) extent.height,
                                                    0.1f, 100.0f);

            // Or, for an ortho camera :
            // glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

            // Camera matrix
            glm::mat4 View = glm::lookAt(
                    newPosition, // Camera is at (4,3,3), in World Space
                    glm::vec3(0, 0, 0), // and looks at the origin
                    glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
            );

            // Model matrix : an identity matrix (model will be at the origin)
            glm::mat4 Model = glm::mat4(1.0f);

            // Our ModelViewProjection : multiplication of our 3 matrices
            glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

            cameraData.MVP = mvp;
        }

        static void createCommandPool(Device &device,
                                      VkCommandPool &commandPool) {
            QueueFamiliesData data = device.getQueueFamiliesData();

            VkCommandPoolCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            createInfo.queueFamilyIndex = data.graphicsQueueFamilyData->index;

            VkResult result = vkCreateCommandPool(device.getDevice(), &createInfo, nullptr, &commandPool);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create command pool") + string_VkResult(result));
            }

            std::cout << "created command pool" << std::endl;
        }

        static void createCommandBuffers(const VkDevice &device, VkCommandPool &commandPool, int maxFramesInFlight,
                                         std::vector<VkCommandBuffer> &commandBuffers) {
            commandBuffers.resize(maxFramesInFlight);

            VkCommandBufferAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocateInfo.commandPool = commandPool;
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

            VkResult result = vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data());

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to allocate command buffers: ") + string_VkResult(result));
            }

            std::cout << "created command buffers" << std::endl;
        }

        static void createDescriptorPool(const VkDevice &device, const VkDescriptorType &descriptorType,
                                         VkDescriptorPool &descriptorPool, int maxFramesInFlight) {

            VkDescriptorPoolSize poolSize{
                    .type = descriptorType,
                    .descriptorCount = static_cast<uint32_t>(maxFramesInFlight)
            };

            //std::vector<VkDescriptorPoolSize> poolSizes(maxFramesInFlight, poolSizes);

            VkDescriptorPoolCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            createInfo.maxSets = maxFramesInFlight;
            createInfo.poolSizeCount = 1;
            createInfo.pPoolSizes = &poolSize;

            VkResult result = vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create descriptor pool: ") + string_VkResult(result));
            }

            std::cout << "created descriptor pool" << std::endl;
        }

        static void createDescriptorSets(const VkDevice &device,
                                         const VkDescriptorPool &descriptorPool,
                                         int maxFramesInFlight,
                                         const VkDescriptorSetLayout &descriptorSetLayout,
                                         const VkDescriptorType &descriptorType,
                                         const VkBuffer &buffer,
                                         std::vector<VkDescriptorSet> &descriptorSets) {
            descriptorSets.resize(maxFramesInFlight);

            std::vector<VkDescriptorSetLayout> descriptorSetLayouts(maxFramesInFlight, descriptorSetLayout);

            VkDescriptorSetAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocateInfo.descriptorPool = descriptorPool;
            allocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSets.size());
            allocateInfo.pSetLayouts = descriptorSetLayouts.data();

            VkResult result = vkAllocateDescriptorSets(device, &allocateInfo, descriptorSets.data());

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create descriptor sets: ") + string_VkResult(result));
            }

            for (int i = 0; i < maxFramesInFlight; i++) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = buffer;
                bufferInfo.offset = 0;
                bufferInfo.range = VK_WHOLE_SIZE;

                VkWriteDescriptorSet writeDescriptorSet;
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.pNext = nullptr;
                writeDescriptorSet.dstSet = descriptorSets[i];
                writeDescriptorSet.dstBinding = 0;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.descriptorType = descriptorType;
                writeDescriptorSet.pImageInfo = nullptr;
                writeDescriptorSet.pBufferInfo = &bufferInfo;
                writeDescriptorSet.pTexelBufferView = nullptr;

                vkUpdateDescriptorSets(
                        device,
                        1,
                        &writeDescriptorSet,
                        0,
                        nullptr);
            }

            std::cout << "created descriptor sets" << std::endl;

        }

        static void createSynchronizationPrimitives(
                const VkDevice &device,
                std::vector<VkSemaphore> &imageAvailableSemaphores,
                std::vector<VkSemaphore> &renderFinishedSemaphores,
                std::vector<VkFence> &inFlightFences,
                int maxFramesInFlight) {
            imageAvailableSemaphores.resize(maxFramesInFlight);
            renderFinishedSemaphores.resize(maxFramesInFlight);
            inFlightFences.resize(maxFramesInFlight);

            VkSemaphoreCreateInfo semaphoreCreateInfo{};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < maxFramesInFlight; i++) {
                VkResult createImageAvailableSemaphoresResult =
                        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]);
                if (createImageAvailableSemaphoresResult != VK_SUCCESS) {
                    throw std::runtime_error(std::string("failed to create image available semaphores") +
                                             string_VkResult(createImageAvailableSemaphoresResult));
                }

                VkResult createRenderFinishedSemaphoresResult =
                        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]);
                if (createRenderFinishedSemaphoresResult != VK_SUCCESS) {
                    throw std::runtime_error(std::string("failed to create render finished semaphores") +
                                             string_VkResult(createRenderFinishedSemaphoresResult));
                }

                VkResult createInFlightFencesResult =
                        vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]);
                if (createInFlightFencesResult != VK_SUCCESS) {
                    throw std::runtime_error(std::string("failed to create in flight fences") +
                                             string_VkResult(createInFlightFencesResult));
                }
            }

            std::cout << "created synchronization primitives" << std::endl;
        }

        template<typename T>
        static void updateBuffer(const VmaAllocator &allocator,
                                 const VkBuffer &buffer,
                                 const VmaAllocation &allocation,
                                 const T &data,
                                 const std::size_t &size) {

            void *mappedData;
            vmaMapMemory(allocator, allocation, &mappedData);
            memcpy(mappedData, &data, size);
            vmaUnmapMemory(allocator, allocation);
        }

        template<typename T>
        static void createBuffer(const VmaAllocator &allocator,
                                 VkBuffer &buffer,
                                 VmaAllocation &allocation,
                                 const T &data,
                                 std::size_t size,
                                 VkBufferUsageFlags usageFlags) {

            VkBufferCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createInfo.size = size; // size in bytes, should be greater than zero
            createInfo.usage = usageFlags;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo allocationCreateInfo{};
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            allocationCreateInfo.requiredFlags =
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            allocationCreateInfo.pool = VK_NULL_HANDLE;
            allocationCreateInfo.priority = 1.0f;

            VmaAllocationInfo allocationInfo;
            VkResult result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &buffer, &allocation,
                                              &allocationInfo);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create buffer: ") + string_VkResult(result));
            }

            void *mappedData;
            vmaMapMemory(allocator, allocation, &mappedData);
            memcpy(mappedData, &data, size);
            vmaUnmapMemory(allocator, allocation);

            std::cout << "created buffer" << std::endl;
        }

        /*
         * Very inefficient
         * Todo: refactor
         */
        static void loadObj(const std::string &filePath,
                            std::vector<VertexData> &vertices,
                            std::vector<uint32_t> &indices) {

            vertices.clear();
            indices.clear();

            tinyobj::ObjReaderConfig config;
            //config.mtl_search_path = "./";

            tinyobj::ObjReader reader;

            if (!reader.ParseFromFile(filePath, config)) {
                if (!reader.Error().empty()) {
                    throw std::runtime_error(
                            std::string("tiny obj reader failed to read from file: ") + reader.Error());
                }
            }

            if (!reader.Warning().empty()) {
                std::cout << "tiny obj reader warning: " << reader.Warning() << std::endl;
            }

            auto &attributes = reader.GetAttrib();
            auto &shapes = reader.GetShapes();
            //auto &materials = reader.GetMaterials();

            for (size_t v = 0; v < attributes.vertices.size(); v += 3) {

                VertexData vertexData{{
                                              attributes.vertices[v + 0],
                                              attributes.vertices[v + 1],
                                              attributes.vertices[v + 2]
                                      }};
                vertices.push_back(vertexData);
            }

            for (size_t s = 0; s < shapes.size(); s++) {
                const std::vector<tinyobj::index_t> &sourceIndices = shapes[s].mesh.indices;

                for (size_t i = 0; i < sourceIndices.size(); i++) {
                    const tinyobj::index_t &index = sourceIndices[i];
                    indices.push_back(index.vertex_index);
                }
            }

            std::cout << "loaded 3d model at: " << filePath << std::endl;

//            for (size_t s = 0; s < shapes.size(); s++) {
//
//                size_t index_offset = 0;
//                for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
//                    size_t vertices = size_t(shapes[s].mesh.num_face_vertices[f]);
//
//                    for (size_t v = 0; v < vertices; v++) {
//                        tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];
//                        size_t a = 3*size_t(index.vertex_index);
//                        tinyobj::real_t x = attributes.vertices[a+0];
//                        tinyobj::real_t y = attributes.vertices[a+1];
//                        tinyobj::real_t z = attributes.vertices[a+2];
//                    }
//                }
//            }
        }

        static void recordCommandBuffer(
                VkCommandBuffer &commandBuffer,
                VkDescriptorSet &descriptorSet,
                uint32_t imageIndex,
                Device &device,
                Swapchain &swapchain,
                GraphicsPipeline &graphicsPipeline,
                RenderPass &renderPass,
                VkBuffer &vertexBuffer,
                VkBuffer &indexBuffer,
                std::vector<uint32_t> indices) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(
                        std::string("failed to begin recording command buffer") + string_VkResult(result));
            }

            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

            VkExtent2D extent = swapchain.getSwapchainExtent();

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass.getRenderPass();
            renderPassInfo.framebuffer = swapchain.getSwapchainFramebuffers()[imageIndex];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapchain.getSwapchainExtent();
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindDescriptorSets(commandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    graphicsPipeline.getGraphicsPipelineLayout(),
                                    0,
                                    1,
                                    &descriptorSet,
                                    0,
                                    nullptr);
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getGraphicsPipeline());

            VkViewport viewport{
                    .x = 0.0f,
                    .y = 0.0f,
                    .width = static_cast<float>(extent.width),
                    .height = static_cast<float>(extent.height),
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f};
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{
                    .offset = {0, 0},
                    .extent = extent};
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            VkDeviceSize vertexBufferOffset = 0;
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &vertexBufferOffset);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
            vkCmdEndRenderPass(commandBuffer);

            VkResult endBufferResult = vkEndCommandBuffer(commandBuffer);

            if (endBufferResult != VK_SUCCESS) {
                throw std::runtime_error(
                        std::string("failed to record command buffer") + string_VkResult(endBufferResult));
            }
        }

        static void drawFrame(uint32_t &currentFrame,
                              int maxFramesInFlight,
                              Device &device,
                              Swapchain &swapchain,
                              GraphicsPipeline &graphicsPipeline,
                              RenderPass &renderPass,
                              std::vector<VkCommandBuffer> &commandBuffers,
                              std::vector<VkDescriptorSet> &descriptorSets,
                              std::vector<VkSemaphore> &imageAvailableSemaphores,
                              std::vector<VkSemaphore> &renderFinishedSemaphores,
                              std::vector<VkFence> &inFlightFences,
                              VkBuffer &vertexBuffer,
                              VkBuffer &indexBuffer,
                              std::vector<uint32_t> indices) {

            vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]);

            uint32_t imageIndex;
            VkResult acquireNextImageResult = vkAcquireNextImageKHR(device.getDevice(), swapchain.getSwapchain(),
                                                                    UINT64_MAX, imageAvailableSemaphores[currentFrame],
                                                                    VK_NULL_HANDLE, &imageIndex);

            switch (acquireNextImageResult) {
                case VK_SUCCESS:
                case VK_SUBOPTIMAL_KHR:
                    break;
                default:
                    throw std::runtime_error(
                            std::string("failed to acquire swapchain image") + string_VkResult(acquireNextImageResult));
            }

            vkResetCommandBuffer(commandBuffers[currentFrame], 0);
            recordCommandBuffer(commandBuffers[currentFrame],
                                descriptorSets[currentFrame],
                                imageIndex,
                                device,
                                swapchain,
                                graphicsPipeline,
                                renderPass,
                                vertexBuffer,
                                indexBuffer,
                                indices);

            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            VkResult queueSubmitResult = vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo,
                                                       inFlightFences[currentFrame]);

            if (queueSubmitResult != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to submit queue") + string_VkResult(queueSubmitResult));
            }

            VkSwapchainKHR swapchains[] = {swapchain.getSwapchain()};

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapchains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            VkResult queuePresentResult = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

            switch (queuePresentResult) {
                case VK_SUBOPTIMAL_KHR:
                    break;
                case VK_SUCCESS:
                    break;
                default:
                    throw std::runtime_error(
                            std::string("failed to present swap chain image") + string_VkResult(queuePresentResult));
            }

            currentFrame = (currentFrame + 1) % maxFramesInFlight;
        }
    };
}

#endif //SPHERE_RENDERER_HPP
