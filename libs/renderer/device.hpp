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

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

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
            createInstance(instance, requiredInstanceLayerNames);
            createSurface(instance, window.window(), surface);
            pickPhysicalDevice(instance, surface, physicalDevice,  queueFamiliesData, requiredDeviceExtensionNames);
            createDevice(physicalDevice, device, queueFamiliesData, requiredDeviceExtensionNames);
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

        const std::vector<const char *> requiredDeviceExtensionNames{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        const std::vector<const char *> requiredInstanceLayerNames{
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
        static void createSurface(const VkInstance &instance, GLFWwindow *window, VkSurfaceKHR &surface) {

            VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create window surface: ") + string_VkResult(result));
            }
        }

        /*
         * Picks the physical device with the highest score and one that is valid.
         * Sets the physicalDevice and queueFamiliesData (i.e. caches the queueFamiliesData in this class)
         */
        static void pickPhysicalDevice(const VkInstance &instance,
                                       const VkSurfaceKHR &surface,
                                       VkPhysicalDevice &physicalDevice,
                                       QueueFamiliesData &queueFamiliesData,
                                       const std::vector<const char *> &requiredDeviceExtensionNames = {}) {

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

                int score = getPhysicalDeviceScore(physicalDevice, surface, physicalDeviceErrorMessage, requiredDeviceExtensionNames);
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
         * Todo: a list of required / preferred device extensions should be supplied.
         * Todo: a list of required / preferred features should be supplied.
         *
         * @param errorMessage When the physical device does not support a required feature, this string will be populated with the error message.
         * @returns The score. 0 if not valid.
         */
        static int getPhysicalDeviceScore(const VkPhysicalDevice &physicalDevice,
                                          const VkSurfaceKHR &surface,
                                          std::string &errorMessage,
                                          const std::vector<const char *> requiredDeviceExtensionNames = {}) {
            int score = 0;

            // get physical device properties
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

            // get physical device features, e.g. robustBufferAccess or geometryShader (all VkBool32)
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(physicalDevice, &features);

            // get physical device extensions
            uint32_t deviceExtensionsCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, nullptr);
            std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionsCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, deviceExtensions.data());

            // see if this physical device supports the required device extensions
            for (const auto &requiredDeviceExtensionName : requiredDeviceExtensionNames) {
                if (*std::find_if(deviceExtensions.begin(),
                                  deviceExtensions.end(),
                                  [&requiredDeviceExtensionName](const VkExtensionProperties extension) -> bool {
                    return strcmp(extension.extensionName, requiredDeviceExtensionName) == 0;
                }) == *deviceExtensions.end()) {
                    // this means the device extension is not supported
                    errorMessage = std::string("physical device does not contain required device extension ") + std::string(requiredDeviceExtensionName);
                    return 0;
                }
            }

//            for (const auto &deviceExtension : deviceExtensions) {
//                std::cout << "Supported device extension: " << deviceExtension.extensionName << std::endl;
//            }

            // vkEnumerateDeviceLayerProperties should be ignored, as device layers are deprecated.

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
        static void createDevice(VkPhysicalDevice &physicalDevice, VkDevice &device, const QueueFamiliesData &queueFamiliesData, const std::vector<const char *> &requiredDeviceExtensionNames = {}) {

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

            // add the required device extensions
            std::vector<const char *> enabledDeviceExtensionNames(requiredDeviceExtensionNames.begin(), requiredDeviceExtensionNames.end());

            // VUID-VkDeviceCreateInfo-pProperties-04451: if a physical device supports VK_KHR_portability_subset, it should be added to the ppEnabledExtensionNames.
            uint32_t deviceExtensionsCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, nullptr);
            std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionsCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, deviceExtensions.data());

            if (*std::find_if(
                    deviceExtensions.begin(),
                    deviceExtensions.end(),
                    [](const VkExtensionProperties extension) -> bool {
                        return strcmp(extension.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0;
                    }) != *deviceExtensions.end()) {
                // because VK_KHR_portability_subset depends on VK_KHR_get_physical_device_properties2, it needs to be added to the device extensions as well.
                enabledDeviceExtensionNames.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
                enabledDeviceExtensionNames.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            }

            // print enabled device extensions
            for (const auto &enabledDeviceExtensionName : enabledDeviceExtensionNames) {
                std::cout << "enabled device extension: " << enabledDeviceExtensionName << std::endl;
            }

            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
            deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensionNames.size());
            deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensionNames.data();
            // deviceCreateInfo.ppEnabledLayerNames and deviceCreateInfo.enabledLayerCount are deprecated. layers are now specified when creating the vulkan instance.

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
        static void getEnabledInstanceExtensions(std::vector<const char *> &enabledExtensionNames,
                                                 uint32_t &enabledExtensionCount,
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
