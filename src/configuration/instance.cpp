
#include "configuration/instance.hpp"

#include "utility/check.hpp"

#include <set>
#include <string>

Instance::Instance(char const *appName, std::vector<const char *> const &validationLayers, VkDebugUtilsMessengerCreateInfoEXT const &debugMessengerCreateInfo)
{
    // Check validation layer support
    if (!checkValidationLayerSupport(validationLayers))
        throw std::exception("Validation layers not supported.");

    // Generate create info
    VkApplicationInfo appInfo
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = appName,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };
    std::vector<const char *> extensions = getRequiredExtensions();
    VkInstanceCreateInfo instanceCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &debugMessengerCreateInfo,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    // Create instance
    check::fail( vkCreateInstance(&instanceCreateInfo, nullptr, &handle), "vkCreateInstance failed." );
}

Instance::~Instance()
{
    vkDestroyInstance(handle, nullptr);
}

VkInstance const &Instance::getHandle() const
{
    return handle;
}

bool Instance::checkValidationLayerSupport(std::vector<const char *> const &validationLayers)
{
    // Get available layers
    uint32_t nAvailableLayers;
    vkEnumerateInstanceLayerProperties(&nAvailableLayers, nullptr);
    std::vector<VkLayerProperties> availableLayers(nAvailableLayers);
    vkEnumerateInstanceLayerProperties(&nAvailableLayers, availableLayers.data());

    // Convert to set
    std::set<std::string> availableLayersSet;
    for (VkLayerProperties const &availableLayer : availableLayers)
        availableLayersSet.insert(availableLayer.layerName);

    // Check support for each validation layer
    for (char const *layerName : validationLayers)
        if (availableLayersSet.count(layerName)==0)
            return false;
    
    return true;
}

std::vector<char const *> Instance::getRequiredExtensions()
{
    // Get GLFW-required extensions
    uint32_t glfwExtensionCount = 0;
    char const **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<char const *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // Add debug extension
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}
