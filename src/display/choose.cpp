
#include "display/choose.hpp"

#include <algorithm>

VkSurfaceFormatKHR choose::swapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats)
{
    for (VkSurfaceFormatKHR const &availableFormat : availableFormats)
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;

    return availableFormats[0];
}

VkPresentModeKHR choose::swapPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes)
{
    for (VkPresentModeKHR const &availablePresentMode : availablePresentModes)
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose::swapExtent(VkSurfaceCapabilitiesKHR const &capabilities, GLFWwindow *window)
{
    // Return default if extent is fixed
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    
    // Return best extent within capabilities' min and max
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return VkExtent2D{
        std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}
