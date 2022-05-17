
#pragma once

#include <vulkan/vulkan.h>

#include <iostream>

class Instance;

class DebugMessenger
{
private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData
    )
    {
        std::cerr << "[VALIDATION LAYER]: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

public:
    static VkDebugUtilsMessengerCreateInfoEXT constexpr debugMessengerCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback
    };

private:
    Instance const *instance;
    VkDebugUtilsMessengerEXT handle;

public:
    DebugMessenger(Instance const *instance);
    ~DebugMessenger();
    VkDebugUtilsMessengerEXT const &getHandle() const;
};
