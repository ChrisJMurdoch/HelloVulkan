
#pragma once

#include "display/physicalDevice.hpp"

#include <vulkan/vulkan.h>

class Device
{
private:
    VkDevice handle;
    VkQueue mainQueue;

public:
    Device(PhysicalDevice const *physicalDevice, uint32_t graphicsQueueFamilyIndex, std::vector<const char*> const &validationLayers, std::vector<const char*> const &extensions);
    ~Device();
    VkDevice const &getHandle() const;
    VkQueue const &getQueue() const;
};
