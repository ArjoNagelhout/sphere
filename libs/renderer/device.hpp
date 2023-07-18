#ifndef SPHERE_DEVICE_HPP
#define SPHERE_DEVICE_HPP

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>

#include "window.hpp"

#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>

namespace renderer {

    const char *ENGINE_NAME = "Sphere";
    const char *APPLICATION_NAME = "Sphere";
    const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 0, 0);
    const uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(1, 0, 0);

    /*
     * Creates a device that can be used from a physical device.
     * Is dependent on a glfw window already being created.
     *
     * Does all vulkan setup related to setting the right extensions, layers, and
     * creating the vulkan instance. 
     */
    class Device {

    public:
        // initialize in constructor (no two-step initialization)
        explicit Device(renderer::Window &window) : window(window) {

            // application info
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.apiVersion = VK_API_VERSION_1_0;
            appInfo.applicationVersion = APPLICATION_VERSION;
            appInfo.engineVersion = ENGINE_VERSION;
            appInfo.pEngineName = ENGINE_NAME;
            appInfo.pApplicationName = APPLICATION_NAME;

            // layers and extensions
            uint32_t extensionsCount;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionsCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

            for (auto const &extension: extensions) {
                std::cout
                        << "extension: " << extension.extensionName
                        << ", specVersion: " << extension.specVersion << '\n';
            }

            uint32_t layersCount;
            vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
            std::vector<VkLayerProperties> layers(layersCount);
            vkEnumerateInstanceLayerProperties(&layersCount, layers.data());

            for (auto const &layer: layers) {
                std::cout
                        << "layer: " << layer.layerName
                        << ", description: " << layer.description
                        << ", implVersion: " << layer.implementationVersion
                        << ", specVersion: " << layer.specVersion << '\n';
            }

            // vulkan instance extensions required for creating vulkan surfaces for glfw windows.
            uint32_t glfwRequiredExtensionsCount;
            const char **glfwRequiredExtensionsArray = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionsCount);
            std::vector<const char *> glfwRequiredExtensions(glfwRequiredExtensionsArray, glfwRequiredExtensionsArray + glfwRequiredExtensionsCount);

            std::cout << "count: " << glfwRequiredExtensionsCount << std::endl;

            for (auto const &glfwRequiredExtension : glfwRequiredExtensions) {
                std::cout << "glfw required extension: " << glfwRequiredExtension << '\n';
            }

            // create instance
            VkInstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &appInfo;

            VkResult createInstanceResult = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
            if (createInstanceResult != VK_SUCCESS) {
                throw std::runtime_error(
                        std::string("failed to create instance: ") +
                        std::string(string_VkResult(createInstanceResult)));
            }
        }

        // cleanup
        ~Device() {
            vkDestroyInstance(instance, nullptr);
        }

    private:
        Window &window;
        VkInstance instance;
    };
}

#endif //SPHERE_DEVICE_HPP
