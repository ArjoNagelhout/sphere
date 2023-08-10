#include "utils.h"

#include <vulkan/vk_enum_string_helper.h>

#include <string>
#include <stdexcept>

namespace renderer {

    // equality operators
    bool operator==(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return strcmp(lhs.extensionName, rhs.extensionName) == 0
               && lhs.specVersion == rhs.specVersion;
    }

    bool operator!=(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return !(lhs == rhs);
    }

    bool operator==(VkLayerProperties &lhs, VkLayerProperties &rhs) {
        return strcmp(lhs.layerName, rhs.layerName) == 0
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

    /**
     * Throws a runtime error when result is not VK_SUCCESS
     * @param result
     */
    void checkResult(VkResult result) {
        if (result != VK_SUCCESS) {
            throw std::runtime_error(std::string("vulkan command failed: ") + string_VkResult(result));
        }
    }
}