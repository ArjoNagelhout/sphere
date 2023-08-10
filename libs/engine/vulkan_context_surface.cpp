#include "vulkan_context.h"

namespace engine {
    /*
     * we use glfw to create a surface, because otherwise we need to call platform specific functions such as
     * vkCreateAndroidSurfaceKHR and vkCreateWaylandSurfaceKHR.
     *
     * A VkSurfaceKHR can be used to present images to the screen.
     */
    void VulkanContext::createSurface() {
        checkResult(glfwCreateWindowSurface(instance, configuration.window, nullptr, &surface));

        std::cout << "created surface" << std::endl;
    }
}