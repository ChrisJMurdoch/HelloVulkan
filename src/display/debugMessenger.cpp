
#include "display/debugMessenger.hpp"

#include "utility/check.hpp"

DebugMessenger::DebugMessenger(Instance const *instance) : instance(instance)
{
    // Retrieve function pointer
    auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance->getHandle(), "vkCreateDebugUtilsMessengerEXT");
    check::null( vkCreateDebugUtilsMessengerEXT, "vkCreateDebugUtilsMessengerEXT null." );
    
    // Create messenger
    check::fail( vkCreateDebugUtilsMessengerEXT(instance->getHandle(), &debugMessengerCreateInfo, nullptr, &handle), "vkCreateDebugUtilsMessengerEXT fail." );
}

DebugMessenger::~DebugMessenger()
{
    // Retrieve function pointer
    auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance->getHandle(), "vkDestroyDebugUtilsMessengerEXT");

    // Destroy messenger
    if (vkDestroyDebugUtilsMessengerEXT != nullptr)
        vkDestroyDebugUtilsMessengerEXT(instance->getHandle(), handle, nullptr);
}

VkDebugUtilsMessengerEXT const &DebugMessenger::getHandle() const
{
    return handle;
}
