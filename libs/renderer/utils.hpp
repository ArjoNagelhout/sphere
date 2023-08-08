#ifndef SPHERE_UTILS_HPP
#define SPHERE_UTILS_HPP

#include <vulkan/vulkan.h>

namespace renderer {

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
}

#endif //SPHERE_UTILS_HPP
