
#pragma once

#include "display/swapchain.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <functional>

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
    void record(Swapchain const *swapchain, int commandBufferIndex, int frameBufferIndex, std::function<void(VkCommandBuffer const &commandBuffer)> commands);
};
