
#pragma once

#include <vulkan/vulkan.hpp>

/** Inline error checking for Vulkan returns */
namespace check
{
    inline void null (void *value, char const *message = "Error: null value.")
    {
        if (value == nullptr)
            throw std::exception(message);
    }

    inline void fail (VkResult value, char const *message = "Error: Vulkan call failed.")
    {
        if (value != VK_SUCCESS)
            throw std::exception(message);
    }

    inline void zero (int value, char const *message = "Error: zero value.")
    {
        if (value == 0)
            throw std::exception(message);
    }
};
