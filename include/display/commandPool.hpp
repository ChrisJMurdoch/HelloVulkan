
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class CommandPool
{
private:
    VkDevice const &device;
    VkCommandPool handle;
    std::vector<VkCommandBuffer> commandBuffers;

public:
    CommandPool(VkDevice const &device, uint32_t const graphicsQueueFamilyIndex, int maxFramesInFlight);
    ~CommandPool();
    VkCommandPool const &getHandle() const;
    std::vector<VkCommandBuffer> const &getBuffers() const;
};
