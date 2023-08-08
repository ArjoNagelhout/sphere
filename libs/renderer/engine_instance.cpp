#include "engine.hpp"

namespace renderer {

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

    void Engine::createInstance(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &requiredLayers) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.applicationVersion = configuration.applicationVersion;
        appInfo.engineVersion = configuration.engineVersion;
        appInfo.pEngineName = configuration.engineName.data();
        appInfo.pApplicationName = configuration.applicationName.data();

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

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vulkanData.instance);

        if (result != VK_SUCCESS) {
            throw std::runtime_error(std::string("failed to create instance: ") + string_VkResult(result));
        }

        std::cout << "created instance" << std::endl;
    }
}