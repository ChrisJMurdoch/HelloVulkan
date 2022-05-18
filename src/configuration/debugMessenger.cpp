
#include "configuration/debugMessenger.hpp"

#include "configuration/instance.hpp"
#include "utility/check.hpp"

DebugMessenger::DebugMessenger(Instance const *instance) : instance(instance)
{
    // Retrieve function pointer
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance->getHandle(), "vkCreateDebugUtilsMessengerEXT"));
    check::null( vkCreateDebugUtilsMessengerEXT, "vkCreateDebugUtilsMessengerEXT null." );
    
    // Create messenger
    check::fail( vkCreateDebugUtilsMessengerEXT(instance->getHandle(), &debugMessengerCreateInfo, nullptr, &handle), "vkCreateDebugUtilsMessengerEXT failed." );
}

DebugMessenger::~DebugMessenger()
{
    // Retrieve function pointer
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance->getHandle(), "vkDestroyDebugUtilsMessengerEXT"));
    check::null( vkDestroyDebugUtilsMessengerEXT, "vkDestroyDebugUtilsMessengerEXT null." );

    // Destroy messenger
    vkDestroyDebugUtilsMessengerEXT(instance->getHandle(), handle, nullptr);
}

VkDebugUtilsMessengerEXT const &DebugMessenger::getHandle() const
{
    return handle;
}
