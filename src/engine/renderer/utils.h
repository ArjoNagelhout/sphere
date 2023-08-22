#ifndef SPHERE_UTILS_H
#define SPHERE_UTILS_H

#include "vulkan.h"

#include <deque>

namespace engine::renderer {

    // equality operators
    bool operator==(VkExtensionProperties &lhs, VkExtensionProperties &rhs);
    bool operator!=(VkExtensionProperties &lhs, VkExtensionProperties &rhs);
    bool operator==(VkLayerProperties &lhs, VkLayerProperties &rhs);
    bool operator!=(VkLayerProperties &lhs, VkLayerProperties &rhs);
    bool operator==(const VkSurfaceFormatKHR &lhs, const VkSurfaceFormatKHR &rhs);
    bool operator!=(const VkSurfaceFormatKHR &lhs, const VkSurfaceFormatKHR &rhs);

    /**
     * Throws a runtime error when result is not VK_SUCCESS
     * @param result
     */
    void checkResult(VkResult result);

    VkExtent3D toExtent3D(const VkExtent2D &extent2D);

    struct DestroyQueue {
        std::deque<std::function<void()>> queue;

        void push(std::function<void()> function);

        void flush();
    };
}

#endif //SPHERE_UTILS_H
