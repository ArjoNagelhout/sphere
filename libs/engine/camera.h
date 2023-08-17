#ifndef SPHERE_CAMERA_H
#define SPHERE_CAMERA_H

#include "memory_allocator.h"
#include "swapchain.h"

#include <glm/mat4x4.hpp>

namespace engine {
    struct CameraData {
        glm::mat4x4 VP;
    };

    class Camera {

    public:
        explicit Camera(MemoryAllocator &allocator, Swapchain &swapchain);

        glm::vec3 position;
        VkBuffer cameraDataBuffer;

        void updateCameraData();
    private:
        MemoryAllocator &allocator;
        Swapchain &swapchain;

        CameraData cameraData{};
        VmaAllocation cameraDataBufferAllocation;
    };
}

#endif //SPHERE_CAMERA_H
