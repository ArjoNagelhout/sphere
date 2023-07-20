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
#include <algorithm>
#include <map>

namespace renderer {

    const char *ENGINE_NAME = "Sphere";
    const char *APPLICATION_NAME = "Sphere";
    const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 0, 0);
    const uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(1, 0, 0);

    bool operator==(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return lhs.extensionName == rhs.extensionName && lhs.specVersion == rhs.specVersion;
    }

    bool operator!=(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return !(lhs == rhs);
    }

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
            createInstance(instance, false);
            pickPhysicalDevice(instance, physicalDevice);
        }

        // cleanup
        ~Device() {
            vkDestroyInstance(instance, nullptr);
        }

    private:
        Window &window;
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        void createInstance(VkInstance &instance, bool enableValidationLayers) {
            // application info
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.apiVersion = VK_API_VERSION_1_0;
            appInfo.applicationVersion = APPLICATION_VERSION;
            appInfo.engineVersion = ENGINE_VERSION;
            appInfo.pEngineName = ENGINE_NAME;
            appInfo.pApplicationName = APPLICATION_NAME;

            uint32_t layersCount;
            vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
            std::vector<VkLayerProperties> layers(layersCount);
            vkEnumerateInstanceLayerProperties(&layersCount, layers.data());

            for (auto const &layer: layers) {
                std::cout
                        << "instance layer: " << layer.layerName
                        << ", description: " << layer.description
                        << ", implVersion: " << layer.implementationVersion
                        << ", specVersion: " << layer.specVersion << '\n';
            }

            VkInstanceCreateFlags flags{};

            uint32_t enabledExtensionCount;
            std::vector<const char *> enabledExtensionNames;
            getEnabledInstanceExtensions(enabledExtensionNames, enabledExtensionCount, flags);

            uint32_t enabledLayerCount;
            std::vector<const char *> enabledLayerNames;
            getEnabledInstanceLayerNames(enabledLayerNames, enabledLayerCount);

            // create instance
            VkInstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &appInfo;
            instanceCreateInfo.enabledExtensionCount = enabledExtensionCount;
            instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
            instanceCreateInfo.ppEnabledLayerNames = enabledLayerNames.data();
            instanceCreateInfo.flags = flags;

            VkResult createInstanceResult = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
            if (createInstanceResult != VK_SUCCESS) {
                throw std::runtime_error(
                        std::string("failed to create instance: ") +
                        std::string(string_VkResult(createInstanceResult)));
            }
        }

        void pickPhysicalDevice(VkInstance &instance, VkPhysicalDevice &physicalDevice) {

            uint32_t physicalDeviceCount;
            vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
            std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
            vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

            if (physicalDeviceCount == 0) {
                throw std::runtime_error("no device found with Vulkan support");
            }

            std::cout << physicalDeviceCount << " devices found with Vulkan support" << std::endl;

            // print devices
            for (auto const &physicalDevice : physicalDevices) {

                // get device properties
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(physicalDevice, &properties);
                std::cout
                        << "physical device: " << properties.deviceName
                        << ", device type: " << string_VkPhysicalDeviceType(properties.deviceType)
                        << std::endl;

                getPhysicalDeviceQueueFamilyProperties(physicalDevice);
            }

            // determine the best physical device that supports all required extensions
            int bestScore = 0;
            VkPhysicalDevice bestPhysicalDevice = nullptr;
            std::string physicalDeviceErrorMessage{"Physical device does not support required extensions and features"};
            for (auto const &physicalDevice : physicalDevices) {

                int score = getPhysicalDeviceScore(physicalDevice, physicalDeviceErrorMessage);
                if (score > 0 && score > bestScore) {
                    bestScore = score;
                    bestPhysicalDevice = physicalDevice;
                }
            }

            if (bestPhysicalDevice == nullptr) {
                throw std::runtime_error(physicalDeviceErrorMessage);
            }

            physicalDevice = bestPhysicalDevice;
        }

        /**
         * Returns the score of a given physical device, so that the best one can be selected.
         *
         * @param errorMessage When the physical device does not support a required feature, this string will be populated with the error message.
         * @returns The score. 0 if not valid.
         */
        int getPhysicalDeviceScore(const VkPhysicalDevice &physicalDevice, std::string &errorMessage) {
            int score = 0;

            // get device properties
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                score += 2000;
            } else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                score += 1000;
            }

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(physicalDevice, &features);

            return score;
        }

        void getPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice &physicalDevice) {
            uint32_t queueFamilyPropertiesCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList(queueFamilyPropertiesCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyPropertiesList.data());

            for (const auto &queueFamilyProperties : queueFamilyPropertiesList) {

                std::cout << "queue family properties: "
                    << string_VkQueueFlags(queueFamilyProperties.queueFlags)
                    << ", queueCount: " << queueFamilyProperties.queueCount
                    << std::endl;
            }
        }

        void createDevice(VkPhysicalDevice &physicalDevice, VkDevice &device) {

            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

            // layer names
            // extension names
            //deviceCreateInfo.

            vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
        }

        void getEnabledInstanceLayerNames(std::vector<const char *> &enabledLayerNames, uint32_t &enabledLayerCount) {
            enabledLayerNames = std::vector<const char *>(0);
            enabledLayerCount = 0;
        }

        /*
         * Returns the enabled instance extension names based on the required glfw extensions
         * and custom supplied extensions.
         */
        void
        getEnabledInstanceExtensions(std::vector<const char *> &enabledExtensionNames, uint32_t &enabledExtensionCount,
                                     VkInstanceCreateFlags &flags) {

            uint32_t extensionsCount;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionsCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

            for (auto const &extension: extensions) {
                std::cout << "instance extension: " << extension.extensionName << ", specVersion: " << extension.specVersion
                          << std::endl;
            }

            // vulkan instance extensions required for creating vulkan surfaces for glfw windows.
            uint32_t glfwRequiredExtensionsCount;
            const char **glfwRequiredExtensionsArray = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionsCount);
            std::vector<const char *> glfwRequiredExtensions(glfwRequiredExtensionsArray,
                                                             glfwRequiredExtensionsArray + glfwRequiredExtensionsCount);

            for (auto const &glfwRequiredExtension: glfwRequiredExtensions) {
                std::cout << "glfw required extension: " << glfwRequiredExtension << std::endl;
            }

            // add glfw required extensions to the enabled extensions
            for (auto const &glfwRequiredExtension: glfwRequiredExtensions) {
                if (*std::find_if(extensions.begin(),
                                           extensions.end(),
                                           [&glfwRequiredExtension](const VkExtensionProperties extension) -> bool {
                                               return (strcmp(extension.extensionName, glfwRequiredExtension) == 0);
                                           }) != *(extensions.end())) {
                    enabledExtensionNames.push_back(glfwRequiredExtension);
                    std::cout << "added glfw required extension: " << glfwRequiredExtension << std::endl;
                } else {
                    throw std::runtime_error("could not add required extension: " + std::string(glfwRequiredExtension));
                }
            }

            // if on macOS, we need to enable the portability subset extension
            if (*std::find_if(extensions.begin(),
                             extensions.end(),
                             [](VkExtensionProperties extension) -> bool {
                                 return (strcmp(extension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0);
                             }) != *extensions.end()) {
                enabledExtensionNames.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                flags = flags | VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

                std::cout << "added portability extension" << std::endl;
            }

            enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
        }
    };
}

#endif //SPHERE_DEVICE_HPP
