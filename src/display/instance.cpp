
#include "display/instance.hpp"

#include "utility/check.hpp"

Instance::Instance(VkDebugUtilsMessengerCreateInfoEXT const &debugMessengerCreateInfo, std::vector<const char *> const &validationLayers)
{
    // Check validation layer support
    if (!checkValidationLayerSupport(validationLayers))
        throw std::exception("Validation layers not supported.");

    // Generate create info
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "HelloVulkan",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };
    std::vector<const char *> extensions = getRequiredExtensions();
    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugMessengerCreateInfo,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    // Create instance
    check::fail( vkCreateInstance(&createInfo, nullptr, &handle), "vkCreateInstance failed." );
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
    // Get layer properties
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Check support for each validation layer
    for (char const *layerName : validationLayers) {

        // Search each property for layer
        bool layerFound = false;
        for (VkLayerProperties const &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

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
