
#pragma once

#include <vulkan/vulkan.h>

#include <functional>

class Device;
class Swapchain;

class CommandBuffer
{
private:
    Device const *device;
    VkCommandBuffer const &handle;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

public:
    CommandBuffer(Device const *device, VkCommandBuffer const &handle);
    CommandBuffer(CommandBuffer &&old);
    ~CommandBuffer();
    VkCommandBuffer const &getHandle() const;
    VkSemaphore const &getImageAvailableSemaphore() const;
    VkSemaphore const &getRenderFinishedSemaphore() const;
    VkFence const &getInFlightFence() const;

    void waitForReady(Device const *device) const;
    void record(std::function<void(VkCommandBuffer const &commandBuffer)> commands);
};
