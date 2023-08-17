#include "engine.h"
#include "camera.h"

#include <glm/gtx/transform.hpp>


namespace engine {

    Camera::Camera(MemoryAllocator &allocator, Swapchain &swapchain) : allocator(allocator), swapchain(swapchain) {

        allocator.createBuffer<CameraData>(cameraDataBuffer,
                                                  cameraDataBufferAllocation,
                                                  cameraData,
                                                  sizeof(cameraData),
                                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }

    void Camera::updateCameraData() {
        // first calculate the VP matrix
        glm::mat4 Projection = glm::perspective(glm::radians(60.0f), (float) swapchain.extent.width / (float) swapchain.extent.height,
                                                0.1f, 1000.0f);

        glm::mat4 translationMatrix = glm::translate(position);
        glm::mat4 rotationMatrix = glm::toMat4(rotation);
        glm::mat4 Camera = translationMatrix * rotationMatrix;

        glm::mat4 View = glm::inverse(Camera);

//        glm::mat4 View = glm::lookAt(
//                position, // Camera is at (4,3,3), in World Space
//                glm::vec3(0, 0, 0), // and looks at the origin
//                glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//        );

        glm::mat4 vp = Projection * View;
        cameraData.VP = vp;

        // then update the buffer
        allocator.updateBuffer<CameraData>(cameraDataBuffer,
                                           cameraData,
                                           sizeof(cameraData));
    }
}