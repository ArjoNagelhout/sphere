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
#include <optional>

namespace renderer {

    const char *ENGINE_NAME = "Sphere";
    const char *APPLICATION_NAME = "Sphere";
    const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 0, 0);
    const uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(1, 0, 0);

    // equality operators
    bool operator==(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return lhs.extensionName == rhs.extensionName && lhs.specVersion == rhs.specVersion;
    }

    bool operator!=(VkExtensionProperties &lhs, VkExtensionProperties &rhs) {
        return !(lhs == rhs);
    }

    bool operator==(VkLayerProperties &lhs, VkLayerProperties &rhs) {
        return lhs.layerName == rhs.layerName && lhs.specVersion == rhs.specVersion && lhs.implementationVersion == rhs.implementationVersion;
    }

    bool operator!=(VkLayerProperties &lhs, VkLayerProperties &rhs) {
        return !(lhs == rhs);
    }

    struct QueueFamilyData {

        uint32_t index;
        VkQueueFamilyProperties properties;

    };

    /*
     * Data for each queue family (e.g. their index)
     */
    struct QueueFamiliesData {
        std::optional<QueueFamilyData> graphicsQueueFamilyData;
        std::optional<QueueFamilyData> presentQueueFamilyData;

        bool isComplete() {
            return graphicsQueueFamilyData.has_value() && presentQueueFamilyData.has_value();
        }
    };

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
            createInstance(instance, validationLayerNames);
            createSurface(instance, window.window(), surface);
            pickPhysicalDevice(instance, surface, physicalDevice,  queueFamiliesData);
            createDevice(physicalDevice, device, queueFamiliesData);
        }

        // cleanup
        ~Device() {
            vkDestroyDevice(device, nullptr);
            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyInstance(instance, nullptr);
        }

        const VkDevice &getDevice() {
            return device;
        }

    private:
        Window &window;
        VkInstance instance;
        VkSurfaceKHR surface;
        QueueFamiliesData queueFamiliesData;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        const std::vector<const char *> validationLayerNames{
            "VK_LAYER_KHRONOS_validation"
        };

        static void createInstance(VkInstance &instance, const std::vector<const char *> &requiredLayerNames = {}) {
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

            // print layers
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
            getEnabledInstanceLayerNames(requiredLayerNames, enabledLayerNames, enabledLayerCount);

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

        /*
         * we use glfw to create a surface, because otherwise we need to call platform specific functions such as
         * vkCreateAndroidSurfaceKHR and vkCreateWaylandSurfaceKHR.
         *
         * A VkSurfaceKHR can be used to present images to the screen.
         */
        static void createSurface(VkInstance &instance, GLFWwindow *window, VkSurfaceKHR &surface) {

            VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create window surface: ") + string_VkResult(result));
            }
        }

        /*
         * Picks the physical device with the highest score and one that is valid.
         * Sets the physicalDevice and queueFamiliesData (i.e. caches the queueFamiliesData in this class)
         */
        static void pickPhysicalDevice(VkInstance &instance, const VkSurfaceKHR &surface, VkPhysicalDevice &physicalDevice, QueueFamiliesData &queueFamiliesData) {

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
            }

            // determine the best physical device that supports all required extensions
            int bestScore = 0;
            VkPhysicalDevice bestPhysicalDevice = nullptr;
            std::string physicalDeviceErrorMessage{"Physical device does not support required extensions and features"};
            for (auto const &physicalDevice : physicalDevices) {

                int score = getPhysicalDeviceScore(physicalDevice, surface, physicalDeviceErrorMessage);
                if (score > 0 && score > bestScore) {
                    bestScore = score;
                    bestPhysicalDevice = physicalDevice;
                }
            }

            if (bestPhysicalDevice == nullptr) {
                throw std::runtime_error(physicalDeviceErrorMessage);
            }

            physicalDevice = bestPhysicalDevice;
            queueFamiliesData = getQueueFamiliesData(physicalDevice, surface);
        }

        /**
         * Returns the score of a given physical device, so that the best one can be selected.
         *
         * @param errorMessage When the physical device does not support a required feature, this string will be populated with the error message.
         * @returns The score. 0 if not valid.
         */
        static int getPhysicalDeviceScore(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface, std::string &errorMessage) {
            int score = 0;

            // get device properties
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                score += 2000;
            } else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                score += 1000;
            }

            // we need a present and a graphics queue
            QueueFamiliesData queueFamiliesData = getQueueFamiliesData(physicalDevice, surface);
            if (!queueFamiliesData.isComplete()) {
                errorMessage = "physical device does not contain both a present and a graphics queue";
                return 0; // early return
            }

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(physicalDevice, &features);

            return score;
        }

        /*
         * Enumerates over the existing queue families that are available on the GPU and populates
         * the QueueFamilyData struct that contains the indices for the queue families.
         *
         * This could later be refactored to be smarter about which queue to use depending on which one has better performance
         * for that specific supported operation. Also use separate queues for graphics and compute or transfering.
         */
        static QueueFamiliesData getQueueFamiliesData(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface) {

            QueueFamiliesData queueFamiliesData{};

            uint32_t queueFamilyPropertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList(queueFamilyPropertyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyPropertiesList.data());

            for (int i = 0; i < queueFamilyPropertyCount; i++) {
                auto const &queueFamilyProperties = queueFamilyPropertiesList[i];

                if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    QueueFamilyData data{};
                    data.index = i;
                    data.properties = queueFamilyProperties;
                    queueFamiliesData.graphicsQueueFamilyData = data;
                }

                VkBool32 presentSupport;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

                if (presentSupport) {
                    QueueFamilyData data{};
                    data.index = i;
                    data.properties = queueFamilyProperties;
                    queueFamiliesData.presentQueueFamilyData = data;
                }

                // if both the present queue index and the graphics queue index are set, stop enumerating.
                if (queueFamiliesData.isComplete()) {
                    break;
                }
            }

            return queueFamiliesData;
        }

        /*
         * Creates a logical device based on the chosen physical device.
         * This is the end of the responsibilities of the device class.
         */
        static void createDevice(VkPhysicalDevice &physicalDevice, VkDevice &device, const QueueFamiliesData &queueFamiliesData) {

            // first create a queue create info for each queue family
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

            // uses a map so that no duplicate entries can exist.
            std::map<uint32_t, QueueFamilyData> queueFamilyDataMap = {
                    {queueFamiliesData.presentQueueFamilyData.value().index, queueFamiliesData.presentQueueFamilyData.value()},
                    {queueFamiliesData.graphicsQueueFamilyData.value().index, queueFamiliesData.graphicsQueueFamilyData.value()}
            };

            float queuePriority = 1.0f;
            for (auto queueFamilyData: queueFamilyDataMap) {
                QueueFamilyData data{queueFamilyData.second};

                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = data.index;
                queueCreateInfo.queueCount = data.properties.queueCount;

                // Within the same device, queues with higher priority may be allotted more processing time than queues
                // with lower priority, the higher priority queue may also execute fully before executing the lower
                // priority queue
                queueCreateInfo.pQueuePriorities = &queuePriority;

                queueCreateInfos.push_back(queueCreateInfo);
            }

            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

            // layer names
            // extension names
            //deviceCreateInfo.

            vkCreateDevice(physicalDevice,&deviceCreateInfo, nullptr, &device);
        }

        static void getEnabledInstanceLayerNames(const std::vector<const char *> &requiredLayerNames, std::vector<const char *> &enabledLayerNames, uint32_t &enabledLayerCount) {
            uint32_t layersCount;
            vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
            std::vector<VkLayerProperties> layers(layersCount);
            vkEnumerateInstanceLayerProperties(&layersCount, layers.data());

            for (auto requiredLayerName : requiredLayerNames) {
                if (*std::find_if(layers.begin(),
                                  layers.end(),
                                  [&requiredLayerName](const VkLayerProperties layer) -> bool {
                                      return strcmp(layer.layerName, requiredLayerName) == 0;
                                  }) != *layers.end()) {
                    std::cout << "Added required layer: " << requiredLayerName << std::endl;
                    enabledLayerNames.push_back(requiredLayerName);
                } else {
                    // couldn't find required validation layer
                    throw std::runtime_error(std::string("Device does not support required layer: ") + std::string(requiredLayerName));
                }
            }

            enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
        }

        /*
         * Returns the enabled instance extension names based on the required glfw extensions
         * and custom supplied extensions.
         */
        static void
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
