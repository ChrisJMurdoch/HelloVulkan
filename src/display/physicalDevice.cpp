
#include "display/physicalDevice.hpp"

#include "utility/check.hpp"

#include <set>

PhysicalDevice::PhysicalDevice(VkDevice const &device, Instance const *instance, VkSurfaceKHR const &surface, std::vector<const char*> const &deviceExtensions) : device(device)
{
    // Get physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::exception("Failed to find GPU with Vulkan support");
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, physicalDevices.data());

    // Select first suitable device
    for (VkPhysicalDevice const &physicalDevice : physicalDevices) {
        if (isDeviceSuitable(physicalDevice, surface, deviceExtensions)) {
            handle = physicalDevice;
            return;
        }
    }
    
    // Throw error if none found
    throw std::exception("Failed to find GPU with Vulkan support");
}

PhysicalDevice::~PhysicalDevice()
{
    
}

VkPhysicalDevice const &PhysicalDevice::getHandle() const
{
    return handle;
}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR const &surface, std::vector<const char*> const &deviceExtensions)
{
    // Get queue support
    try
    {
        getGraphicsQueueFamilyIndex(physicalDevice, surface);
    }
    catch(std::exception const &e)
    {
        return false;
    }

    // Check extension support
    if ( !checkDeviceExtensionSupport(physicalDevice, deviceExtensions) )
        return false;
    
    // Check swapchain support
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
    if ( swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        return false;

    return true;
}

bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> const &deviceExtensions)
{
    // Get available extensions
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // Get required extensions
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    // Check each required extension is available
    for (VkExtensionProperties const &extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);
    return requiredExtensions.empty();
}

uint32_t PhysicalDevice::getGraphicsQueueFamilyIndex(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface)
{
    // Retrieve queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // Check each queue family for compatibility
    for (uint32_t i=0; i<queueFamilies.size(); i++)
    {
        VkQueueFamilyProperties const &queueFamily = queueFamilies[i];

        // Check for graphics and present capabilities
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if ( (queueFamily.queueFlags&VK_QUEUE_GRAPHICS_BIT) && presentSupport)
            return i;
    }

    throw std::exception("Couldn't find a queue family with present and graphics capabilities.");
}

SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface)
{
    SwapChainSupportDetails details;

    // Get capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    // Get surface formats
    uint32_t nFormats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &nFormats, nullptr);
    details.formats.resize(nFormats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &nFormats, details.formats.data());

    // Get present modes
    uint32_t nPresentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &nPresentModes, nullptr);
    details.presentModes.resize(nPresentModes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &nPresentModes, details.presentModes.data());

    return details;
}
