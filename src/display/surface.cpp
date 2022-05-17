
#include "display/surface.hpp"

#include "display/instance.hpp"
#include "display/window.hpp"
#include "display/physicalDevice.hpp"
#include "utility/check.hpp"

Surface::Surface(Instance const *instance, Window const *window) : instance(instance)
{
    check::fail( glfwCreateWindowSurface(instance->getHandle(), window->getHandle(), nullptr, &handle), "glfwCreateWindowSurface failed." );
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(instance->getHandle(), handle, nullptr);
}

VkSurfaceKHR const &Surface::getHandle() const
{
    return handle;
}

VkSurfaceCapabilitiesKHR const Surface::getCapabilities(VkPhysicalDevice const &physicalDeviceHandle) const
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceHandle, handle, &capabilities);
    return capabilities;
}

std::vector<VkSurfaceFormatKHR> const Surface::getFormats(VkPhysicalDevice const &physicalDeviceHandle) const
{
    uint32_t nFormats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle, handle, &nFormats, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(nFormats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle, handle, &nFormats, formats.data());
    return formats;
}

std::vector<VkPresentModeKHR> const Surface::getPresentModes(VkPhysicalDevice const &physicalDeviceHandle) const
{
    uint32_t nPresentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle, handle, &nPresentModes, nullptr);

    std::vector<VkPresentModeKHR> presentModes(nPresentModes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle, handle, &nPresentModes, presentModes.data());
    return presentModes;
}

bool Surface::getPresentSupport(VkPhysicalDevice const &physicalDeviceHandle, uint32_t queueFamilyIndex) const
{
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDeviceHandle, queueFamilyIndex, handle, &presentSupport);
    return presentSupport;
}
