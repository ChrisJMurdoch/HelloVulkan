
#pragma once

#include "display/instance.hpp"

#include <vulkan/vulkan.h>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice
{
private:
    VkDevice const &device;
    VkPhysicalDevice handle;

public:
    PhysicalDevice(VkDevice const &device, Instance const *instance, VkSurfaceKHR const &surface, std::vector<const char*> const &deviceExtensions);
    ~PhysicalDevice();
    VkPhysicalDevice const &getHandle() const;

    bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR const &surface, std::vector<const char*> const &deviceExtensions);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> const &deviceExtensions);
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface);

    static uint32_t getGraphicsQueueFamilyIndex(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface);
};
