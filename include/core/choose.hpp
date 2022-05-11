
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace choose
{
    VkSurfaceFormatKHR swapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats);

    VkPresentModeKHR swapPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes);

    VkExtent2D swapExtent(VkSurfaceCapabilitiesKHR const &capabilities, GLFWwindow *window);
}
