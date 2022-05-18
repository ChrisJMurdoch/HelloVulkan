
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Instance
{
private:
    VkInstance handle;

public:
    Instance(char const *appName, std::vector<const char *> const &validationLayers, VkDebugUtilsMessengerCreateInfoEXT const &debugMessengerCreateInfo);
    ~Instance();
    
    VkInstance const &getHandle() const;

private:
    bool checkValidationLayerSupport(std::vector<const char *> const &validationLayers);
    std::vector<char const *> getRequiredExtensions();
};
