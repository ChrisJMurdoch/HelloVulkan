
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Instance
{
private:
    VkInstance handle;

public:
    Instance(VkDebugUtilsMessengerCreateInfoEXT const &debugMessengerCreateInfo, std::vector<const char *> const &validationLayers);
    ~Instance();
    VkInstance const &getHandle() const;
    bool checkValidationLayerSupport(std::vector<const char *> const &validationLayers);
    std::vector<char const *> getRequiredExtensions();
};
