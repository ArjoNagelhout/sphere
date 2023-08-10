#include "engine.h"

#include <map>

namespace renderer {

    /*
     * Enumerates over the existing queue families that are available on the GPU and populates
     * the QueueFamilyData struct that contains the indices for the queue families.
     *
     * This could later be refactored to be smarter about which queue to use depending on which one has better performance
     * for that specific supported operation. Also use separate queues for graphics and compute or transfering.
     */
    QueueFamiliesData getQueueFamiliesData(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface) {

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

    SurfaceData getSurfaceData(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface) {

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

    /**
     * Returns the score of a given physical device, so that the best one can be selected.
     *
     * Todo: a list of required / preferred features should be supplied.
     *
     * @param errorMessage When the physical device does not support a required feature, this string will be populated with the error message.
     * @returns The score. 0 if not valid.
     */
    int getPhysicalDeviceScore(const VkPhysicalDevice &physicalDevice,
                               const VkSurfaceKHR &surface,
                               std::string &errorMessage,
                               const std::vector<const char *> &requiredDeviceExtensionNames) {
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

    PhysicalDeviceData getPhysicalDeviceData(const VkPhysicalDevice &physicalDevice) {

        VkPhysicalDeviceProperties2 properties{};
        properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

        VkPhysicalDevicePortabilitySubsetPropertiesKHR portabilitySubsetProperties{};
        portabilitySubsetProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;
        properties.pNext = &portabilitySubsetProperties;

        vkGetPhysicalDeviceProperties2(physicalDevice, &properties);

        PhysicalDeviceData physicalDeviceData{
                .minVertexInputBindingStrideAlignment = portabilitySubsetProperties.minVertexInputBindingStrideAlignment,
                .maxVertexInputBindingStride = properties.properties.limits.maxVertexInputBindingStride
        };

        return physicalDeviceData;
    }

    /*
     * Picks the physical device with the highest score and one that is valid.
     * Sets the physicalDevice and queueFamiliesData (i.e. caches the queueFamiliesData in this class)
     */
    void Engine::pickPhysicalDevice(const std::vector<const char *> &requiredExtensions) {
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

            int score = getPhysicalDeviceScore(physicalDevice, surface, physicalDeviceErrorMessage, requiredExtensions);
            if (score > 0 && score > bestScore) {
                bestScore = score;
                bestPhysicalDevice = physicalDevice;
            }
        }

        if (bestPhysicalDevice == nullptr) {
            throw std::runtime_error(physicalDeviceErrorMessage);
        }

        physicalDevice = bestPhysicalDevice;

        // cache data
        queueFamiliesData = getQueueFamiliesData(physicalDevice, surface);
        surfaceData = getSurfaceData(physicalDevice, surface);
        physicalDeviceData = getPhysicalDeviceData(physicalDevice);

        // print picked device
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        std::cout << "picked device: " << properties.deviceName << std::endl;
    }

    std::vector<const char *> getEnabledDeviceExtensions(VkPhysicalDevice &physicalDevice, const std::vector<const char *> &requiredExtensions) {

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

    /*
     * Creates a logical device based on the chosen physical device.
     * This is the end of the responsibilities of the device class.
     */
    void Engine::createDevice(const std::vector<const char *> &requiredExtensions) {

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
}