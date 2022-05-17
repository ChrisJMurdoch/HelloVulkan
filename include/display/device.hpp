
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class PhysicalDevice;
class Queue;

class Device
{
private:
    VkDevice handle;
    VkQueue graphicsQueue;

public:
    Device(PhysicalDevice const *physicalDevice, std::vector<const char*> const &validationLayers, std::vector<const char*> const &extensions);
    ~Device();
    VkDevice const &getHandle() const;
    Queue getGraphicsQueue();
};
