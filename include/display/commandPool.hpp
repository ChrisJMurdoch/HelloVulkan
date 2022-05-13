
#pragma once

#include <vulkan/vulkan.h>

#include "display/common.hpp"

#include <vector>

class CommandPool
{
private:
    VkDevice const &device;
    VkCommandPool handle;
    std::vector<VkCommandBuffer> commandBuffers;

public:
    CommandPool(VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, int maxFramesInFlight);
    ~CommandPool();
    VkCommandPool const &getHandle() const;
    std::vector<VkCommandBuffer> const &getBuffers() const;
};
