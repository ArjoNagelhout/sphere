#ifndef SPHERE_UTILS_HPP
#define SPHERE_UTILS_HPP

#include <vulkan/vulkan.h>

namespace renderer {

    // equality operators
    bool operator==(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return lhs.extensionName == rhs.extensionName
            && lhs.specVersion == rhs.specVersion;
    }

    bool operator!=(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return !(lhs == rhs);
    }

    bool operator==(VkLayerProperties &lhs, VkLayerProperties &rhs) {
        return lhs.layerName == rhs.layerName
            && lhs.specVersion == rhs.specVersion
            && lhs.implementationVersion == rhs.implementationVersion;
    }

    bool operator!=(VkLayerProperties &lhs, VkLayerProperties &rhs) {
        return !(lhs == rhs);
    }

    bool operator==(const VkSurfaceFormatKHR &lhs, const VkSurfaceFormatKHR &rhs) {
        return lhs.format == rhs.format
            && lhs.colorSpace == rhs.colorSpace;
    }

    bool operator!=(const VkSurfaceFormatKHR &lhs, const VkSurfaceFormatKHR &rhs) {
        return !(lhs == rhs);
    }
}

#endif //SPHERE_UTILS_HPP
