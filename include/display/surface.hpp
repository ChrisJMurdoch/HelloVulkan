
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Instance;
class Window;
class PhysicalDevice;

class Surface
{
private:
    Instance const *instance;
    VkSurfaceKHR handle;

public:
    Surface(Instance const *instance, Window const *window);
    ~Surface();
    VkSurfaceKHR const &getHandle() const;

    VkSurfaceCapabilitiesKHR const getCapabilities(VkPhysicalDevice const &physicalDeviceHandle) const;
    std::vector<VkSurfaceFormatKHR> const getFormats(VkPhysicalDevice const &physicalDeviceHandle) const;
    std::vector<VkPresentModeKHR> const getPresentModes(VkPhysicalDevice const &physicalDeviceHandle) const;
    bool getPresentSupport(VkPhysicalDevice const &physicalDeviceHandle, uint32_t queueFamilyIndex) const;
};
