#ifndef SPHERE_DEVICE_HPP
#define SPHERE_DEVICE_HPP

#define GLFW_INCLUDE_VULKAN

#include <glfw/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>

#include "utils.hpp"
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

    struct QueueFamilyData {
        uint32_t index;
        VkQueueFamilyProperties properties;
    };

    struct SurfaceData {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkPresentModeKHR> surfacePresentModes;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
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
     *
     * Also creates a surface to draw on.
     */
    class Device {

    public:
        // initialize in constructor (no two-step initialization)
        explicit Device(Window &window, bool debug) : debug(debug), window(window) {

            std::vector<const char *> allRequiredInstanceExtensions{requiredInstanceExtensions.begin(), requiredInstanceExtensions.end()};
            std::vector<const char *> allRequiredInstanceLayers{requiredInstanceLayers.begin(), requiredInstanceLayers.end()};

            if (debug) {
                allRequiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                allRequiredInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
            }

            createInstance(instance, allRequiredInstanceLayers, allRequiredInstanceExtensions);

            if (debug) {
                createDebugMessenger(instance, debugMessenger);
            }

            createSurface(instance, window.getWindow(), surface);
            pickPhysicalDevice(instance, surface, physicalDevice,  queueFamiliesData, requiredDeviceExtensions);
            createDevice(physicalDevice, device, queueFamiliesData, graphicsQueue, presentQueue, requiredDeviceExtensions);
        }

        // cleanup
        ~Device() {
            vkDestroyDevice(device, nullptr);
            vkDestroySurfaceKHR(instance, surface, nullptr);

            if (debug) {
                destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }

            vkDestroyInstance(instance, nullptr);
        }

        const VkDevice &getDevice() {
            return device;
        }

        const VkSurfaceKHR &getSurface() {
            return surface;
        }

        const VkQueue &getGraphicsQueue() {
            return graphicsQueue;
        }

        const VkQueue &getPresentQueue() {
            return presentQueue;
        }

        SurfaceData getSurfaceData() {
            return getSurfaceData(physicalDevice, surface);
        }

        QueueFamiliesData getQueueFamiliesData() {
            return queueFamiliesData;
        }

    private:
        const bool debug;
        Window &window;

        VkInstance instance;
        VkSurfaceKHR surface;
        QueueFamiliesData queueFamiliesData;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkDebugUtilsMessengerEXT debugMessenger;

        const std::vector<const char *> requiredInstanceExtensions{

        };

        const std::vector<const char *> requiredInstanceLayers{

        };

        const std::vector<const char *> requiredDeviceExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        static void createInstance(VkInstance &instance,
                                   const std::vector<const char *> &requiredLayers,
                                   const std::vector<const char *> &requiredExtensions) {
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.apiVersion = VK_API_VERSION_1_0;
            appInfo.applicationVersion = APPLICATION_VERSION;
            appInfo.engineVersion = ENGINE_VERSION;
            appInfo.pEngineName = ENGINE_NAME;
            appInfo.pApplicationName = APPLICATION_NAME;

            VkInstanceCreateFlags flags{};
            std::vector<const char *> enabledExtensions = getEnabledInstanceExtensions(requiredExtensions, flags);
            std::vector<const char *> enabledLayers = getEnabledInstanceLayers(requiredLayers);

            for (const auto &enabledLayer : enabledLayers) {
                std::cout << "enabled instance layer: " << enabledLayer << std::endl;
            }

            for (auto const &enabledExtension : enabledExtensions) {
                std::cout << "enabled instance extension: " << enabledExtension << std::endl;
            }

            VkInstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &appInfo;
            instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
            instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();
            instanceCreateInfo.flags = flags;

            VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create instance: ") + string_VkResult(result));
            }

            std::cout << "created instance" << std::endl;
        }

        static std::vector<const char *> getEnabledInstanceLayers(const std::vector<const char *> &requiredLayers) {
            std::vector<const char *> enabledLayers(0);

            uint32_t layersCount;
            vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
            std::vector<VkLayerProperties> layers(layersCount);
            vkEnumerateInstanceLayerProperties(&layersCount, layers.data());

            for (auto requiredLayer : requiredLayers) {
                if (*std::find_if(layers.begin(),
                                  layers.end(),
                                  [&requiredLayer](const VkLayerProperties layer) -> bool {
                                      return strcmp(layer.layerName, requiredLayer) == 0;
                                  }) != *layers.end()) {
                    enabledLayers.push_back(requiredLayer);
                } else {
                    // couldn't find required layer
                    throw std::runtime_error(std::string("device does not support required layer: ") + std::string(requiredLayer));
                }
            }

            return enabledLayers;
        }

        /*
         * Returns the enabled instance extension names based on the required glfw extensions
         * and custom supplied extensions
         *
         * Also sets flags
         */
        static std::vector<const char *> getEnabledInstanceExtensions(const std::vector<const char *> &requiredExtensions,
                                                 VkInstanceCreateFlags &flags) {
            std::vector<const char *> enabledExtensions(0);

            uint32_t extensionsCount;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionsCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

            // vulkan instance extensions required for creating vulkan surfaces for glfw windows.
            uint32_t glfwRequiredExtensionsCount;
            const char **glfwRequiredExtensionsArray = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionsCount);

            std::vector<const char *> allRequiredExtensions;
            allRequiredExtensions.reserve(requiredExtensions.size() + glfwRequiredExtensionsCount);
            allRequiredExtensions.insert(allRequiredExtensions.end(), requiredExtensions.begin(), requiredExtensions.end());
            allRequiredExtensions.insert(allRequiredExtensions.end(), glfwRequiredExtensionsArray, glfwRequiredExtensionsArray + glfwRequiredExtensionsCount); // we can use pointers as an iterator

            // add required extensions to the enabled extensions if they exist
            for (auto const &requiredExtension: allRequiredExtensions) {
                if (*std::find_if(extensions.begin(),
                                  extensions.end(),
                                  [&requiredExtension](const VkExtensionProperties extension) -> bool {
                                      return (strcmp(extension.extensionName, requiredExtension) == 0);
                                  }) != *(extensions.end())) {
                    enabledExtensions.push_back(requiredExtension);
                } else {
                    throw std::runtime_error("could not add required instance extension: " + std::string(requiredExtension));
                }
            }

            // if on macOS, we need to enable the portability subset extension
            if (*std::find_if(extensions.begin(),
                              extensions.end(),
                              [](VkExtensionProperties extension) -> bool {
                                  return (strcmp(extension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0);
                              }) != *extensions.end()) {
                enabledExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                enabledExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME); // needs to be enabled as well
                flags = flags | VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            }

            return enabledExtensions;
        }

        static void createDebugMessenger(const VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger) {
            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity =
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType =
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = debugCallback;

            VkResult result = createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create debug messenger") + string_VkResult(result));
            }

            std::cout << "created debug messenger" << std::endl;
        }

        // Proxy function that looks up the address of the function, because it's part of an extension
        static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                              const VkAllocationCallbacks *pAllocator,
                                              VkDebugUtilsMessengerEXT *pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        // Again, a proxy function because it's part of an extension and can't be directly called
        static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                           const VkAllocationCallbacks *pAllocator) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                    "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(instance, debugMessenger, pAllocator);
            }
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {

            std::cerr << "validation layer output: "
                      << pCallbackData->pMessage
                      << ", severity:" << string_VkDebugUtilsMessageSeverityFlagBitsEXT(messageSeverity)
                      << ", type: " << string_VkDebugUtilsMessageTypeFlagsEXT(messageType)
                      << std::endl;

            return VK_FALSE;
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

            std::cout << "created surface" << std::endl;
        }

        static SurfaceData getSurfaceData(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface) {

            // get surface capabilities
            VkSurfaceCapabilitiesKHR surfaceCapabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

            // get present modes
            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

            // get formats
            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> formats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

            return {surfaceCapabilities, presentModes, formats};
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

            // print picked device
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            std::cout << "picked device: " << properties.deviceName << std::endl;
        }

        /**
         * Returns the score of a given physical device, so that the best one can be selected.
         *
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

            // make sure physical device has support for drawing onto the given surface
            SurfaceData surfaceData = getSurfaceData(physicalDevice, surface);

            if (surfaceData.surfaceFormats.empty()) {
                errorMessage = "physical device has no supported surface formats";
                return 0;
            } else if (surfaceData.surfacePresentModes.empty()) {
                errorMessage = "physical device has no supported present modes";
                return 0;
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
        static void createDevice(
                VkPhysicalDevice &physicalDevice,
                VkDevice &device,
                const QueueFamiliesData &queueFamiliesData,
                VkQueue &graphicsQueue,
                VkQueue &presentQueue,
                const std::vector<const char *> &requiredDeviceExtensions) {

            // first create a queue create info for each queue family
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

            // uses a map so that no duplicate entries can exist.
            std::map<uint32_t, QueueFamilyData> queueFamilyDataMap = {
                    {queueFamiliesData.presentQueueFamilyData.value().index, queueFamiliesData.presentQueueFamilyData.value()},
                    {queueFamiliesData.graphicsQueueFamilyData.value().index, queueFamiliesData.graphicsQueueFamilyData.value()}
            };

            float queuePriority = 1.0f;
            for (auto queueFamilyData: queueFamilyDataMap) {
                const QueueFamilyData &data = queueFamilyData.second;
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

            std::vector<const char *> enabledDeviceExtensions = getEnabledDeviceExtensions(physicalDevice, requiredDeviceExtensions);

            // print enabled device extensions
            for (const auto &enabledDeviceExtension : enabledDeviceExtensions) {
                std::cout << "enabled device extension: " << enabledDeviceExtension << std::endl;
            }

            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
            deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
            deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
            // deviceCreateInfo.ppEnabledLayerNames and deviceCreateInfo.enabledLayerCount are deprecated. layers are now specified when creating the vulkan instance.

            VkResult result = vkCreateDevice(physicalDevice,&deviceCreateInfo, nullptr, &device);

            if (result != VK_SUCCESS) {
                throw std::runtime_error(std::string("failed to create logical device: ") + string_VkResult(result));
            }

            // get the first queues of the queue families for now.
            vkGetDeviceQueue(device, queueFamiliesData.graphicsQueueFamilyData->index, 0, &graphicsQueue);
            vkGetDeviceQueue(device, queueFamiliesData.presentQueueFamilyData->index, 0, &presentQueue);

            std::cout << "created logical device" << std::endl;
        }

        static std::vector<const char *> getEnabledDeviceExtensions(VkPhysicalDevice &physicalDevice, const std::vector<const char *> &requiredExtensions) {

            // add the required device extensions
            std::vector<const char *> enabledDeviceExtensions(0);

            uint32_t deviceExtensionsCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, nullptr);
            std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionsCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, deviceExtensions.data());

            // add required device extensions if supported
            for (const auto &requiredExtension : requiredExtensions) {
                if (*std::find_if(
                        deviceExtensions.begin(),
                        deviceExtensions.end(),
                        [&requiredExtension](const VkExtensionProperties extension) -> bool {
                            return strcmp(extension.extensionName, requiredExtension) == 0;
                        }) != *deviceExtensions.end()) {
                    enabledDeviceExtensions.push_back(requiredExtension);
                } else {
                    throw std::runtime_error("could not add required device extension: " + std::string(requiredExtension));
                }
            }

            // VUID-VkDeviceCreateInfo-pProperties-04451: if a physical device supports VK_KHR_portability_subset, it should be added to the ppEnabledExtensionNames.
            if (*std::find_if(
                    deviceExtensions.begin(),
                    deviceExtensions.end(),
                    [](const VkExtensionProperties extension) -> bool {
                        return strcmp(extension.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0;
                    }) != *deviceExtensions.end()) {
                enabledDeviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
            }

            return enabledDeviceExtensions;
        }
    };
}

#endif //SPHERE_DEVICE_HPP
