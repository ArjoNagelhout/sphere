#ifndef SPHERE_UTILS_H
#define SPHERE_UTILS_H

#include "includes.h"

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

        void push(const std::function<void()> &function) {
            queue.push_back(function);
        }

        void flush() {
            // rbegin and rend = reverse
            for (auto iterator = queue.rbegin(); iterator != queue.rend(); iterator++) {
                (*iterator)();
            }

            queue.clear();
        }
    };
}

#endif //SPHERE_UTILS_H
