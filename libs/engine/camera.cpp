#include "engine.h"
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

    Camera::Camera(MemoryAllocator &allocator, Swapchain &swapchain) : allocator(allocator), swapchain(swapchain) {

        allocator.createBuffer<CameraData>(cameraDataBuffer,
                                                  cameraDataBufferAllocation,
                                                  cameraData,
                                                  sizeof(cameraData),
                                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }

    void Camera::updateCameraData() {
        // first calculate the MVP matrix
        glm::mat4 Projection = glm::perspective(glm::radians(60.0f), (float) swapchain.extent.width / (float) swapchain.extent.height,
                                                0.1f, 1000.0f);

        // Or, for an ortho camera :
        // glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

        // Camera matrix
        glm::mat4 View = glm::lookAt(
                position, // Camera is at (4,3,3), in World Space
                glm::vec3(0, 0, 0), // and looks at the origin
                glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        // Model matrix : an identity matrix (model will be at the origin)
//        glm::mat4 Model = glm::mat4(1.0f);

        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 vp = Projection * View; // Remember, matrix multiplication is the other way around
        cameraData.VP = vp;

        // then update the buffer
        allocator.updateBuffer<CameraData>(cameraDataBuffer,
                                           cameraData,
                                           sizeof(cameraData));
    }
}