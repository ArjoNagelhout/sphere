#ifndef SPHERE_CAMERA_H
#define SPHERE_CAMERA_H

#include "buffer.h"
#include "swapchain.h"

#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"

namespace engine::renderer {
    struct CameraData {
        glm::mat4x4 VP;
    };

    class Camera {

    public:
        explicit Camera(Swapchain &swapchain);

        glm::vec3 position;
        glm::quat rotation{0, 0, 0, 1};
        Buffer cameraDataBuffer;

        void updateCameraData();
    private:
        Swapchain &swapchain;

        CameraData cameraData{};
    };
}

#endif //SPHERE_CAMERA_H
