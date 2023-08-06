#ifndef SPHERE_CAMERA_HPP
#define SPHERE_CAMERA_HPP

#include <glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

namespace renderer {

    /*
     * Todo: refactor to be a node in the scene hierarchy
     */
    class Camera {

    public:
        explicit Camera() {

        }

        ~Camera() {

        }

    private:
        glm::vec3 position;
        glm::quat rotation;

        /*
         * MVP projection, model is set by model itself.
         */
        glm::mat4x4 getVP() {

        }
    };
}

#endif //SPHERE_CAMERA_HPP
