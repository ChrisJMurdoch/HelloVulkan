
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Instance;
class Surface;

class PhysicalDevice
{
private:
    VkPhysicalDevice handle;
    uint32_t graphicsQueueFamilyIndex;

public:
    PhysicalDevice(Instance const *instance, Surface const *surface, std::vector<const char*> const &deviceExtensions);
    VkPhysicalDevice const &getHandle() const;
    uint32_t getGraphicsQueueFamilyIndex() const;

private:
    static bool checkDeviceSuitability(VkPhysicalDevice const &physicalDeviceHandle, Surface const *surface, std::vector<const char*> const &deviceExtensions);
    static uint32_t calcGraphicsQueueFamilyIndex(VkPhysicalDevice const &physicalDeviceHandle, Surface const *surface);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice const &physicalDeviceHandle, std::vector<const char*> const &deviceExtensions);
};
