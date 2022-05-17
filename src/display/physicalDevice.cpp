
#include "display/physicalDevice.hpp"

#include "display/instance.hpp"
#include "display/surface.hpp"
#include "utility/check.hpp"

#include <iostream>
#include <set>

PhysicalDevice::PhysicalDevice(Instance const *instance, Surface const *surface, std::vector<const char*> const &deviceExtensions)
{
    // Get physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, nullptr);
    check::zero(deviceCount, "Failed to find GPU with Vulkan support.");
    std::vector<VkPhysicalDevice> physicalDeviceHandles(deviceCount);
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, physicalDeviceHandles.data());

    // Select first suitable device
    for (VkPhysicalDevice const &physicalDeviceHandle : physicalDeviceHandles)
    {
        if (checkDeviceSuitability(physicalDeviceHandle, surface, deviceExtensions))
        {
            // Set handle
            handle = physicalDeviceHandle;

            // Cache main queue family index
            mainQueueFamilyIndex = calcMainQueueFamilyIndex(handle, surface);

            // Display device name
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(handle, &physicalDeviceProperties);
            std::cout << "Selected device: " << physicalDeviceProperties.deviceName << std::endl;
            return;
        }
    }

    // Throw error if no suitable device found
    throw std::exception("Failed to find GPU with suitable Vulkan support.");
}

VkPhysicalDevice const &PhysicalDevice::getHandle() const
{
    return handle;
}

uint32_t PhysicalDevice::getMainQueueFamilyIndex() const
{
    return mainQueueFamilyIndex;
}

bool PhysicalDevice::checkDeviceSuitability(VkPhysicalDevice const &physicalDeviceHandle, Surface const *surface, std::vector<const char*> const &deviceExtensions)
{
    // Check queue support
    try
    {
        calcMainQueueFamilyIndex(physicalDeviceHandle, surface);
    }
    catch(std::exception const &e)
    {
        return false;
    }

    // Check extension support
    if ( !checkDeviceExtensionSupport(physicalDeviceHandle, deviceExtensions) )
        return false;

    // Check swapchain support
    if (surface->getFormats(physicalDeviceHandle).empty() || surface->getPresentModes(physicalDeviceHandle).empty())
        return false;

    return true;
}

uint32_t PhysicalDevice::calcMainQueueFamilyIndex(VkPhysicalDevice const &physicalDeviceHandle, Surface const *surface)
{
    // Retrieve queue families
    uint32_t nQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle, &nQueueFamilies, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(nQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle, &nQueueFamilies, queueFamilies.data());

    // Check each queue family for graphics, transfer and present support
    for (uint32_t i=0; i<queueFamilies.size(); i++)
        if (
            queueFamilies[i].queueFlags&VK_QUEUE_GRAPHICS_BIT &&
            queueFamilies[i].queueFlags&VK_QUEUE_TRANSFER_BIT &&
            surface->getPresentSupport(physicalDeviceHandle, i)
        )
            return i;

    throw std::exception("Couldn't find a queue family with graphics, transfer and present capabilities.");
}

bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice const &physicalDeviceHandle, std::vector<const char*> const &deviceExtensions)
{
    // Get available extensions
    uint32_t nAvailableExtensions;
    vkEnumerateDeviceExtensionProperties(physicalDeviceHandle, nullptr, &nAvailableExtensions, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(nAvailableExtensions);
    vkEnumerateDeviceExtensionProperties(physicalDeviceHandle, nullptr, &nAvailableExtensions, availableExtensions.data());

    // Convert to set
    std::set<std::string> availableExtensionsSet;
    for (VkExtensionProperties const &availableExtension : availableExtensions)
        availableExtensionsSet.insert(availableExtension.extensionName);
    
    // Check each required extension is available
    for (const char *extensionName : deviceExtensions)
        if (availableExtensionsSet.count(extensionName)==0)
            return false;
    
    return true;
}
