
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Instance;
class Surface;

class PhysicalDevice
{
private:
    VkPhysicalDevice handle;

public:
    PhysicalDevice(Instance const *instance, Surface const *surface, std::vector<const char*> const &deviceExtensions);
    VkPhysicalDevice const &getHandle() const;

    static uint32_t getGraphicsQueueFamilyIndex(VkPhysicalDevice const &physicalDeviceHandle, Surface const *surface);

private:
    static bool checkDeviceSuitability(VkPhysicalDevice const &physicalDeviceHandle, Surface const *surface, std::vector<const char*> const &deviceExtensions);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice const &physicalDeviceHandle, std::vector<const char*> const &deviceExtensions);
};
