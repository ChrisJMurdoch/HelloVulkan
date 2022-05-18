
#pragma once

#include "command/commandBuffer.hpp"

#include <vulkan/vulkan.h>

class Device;

class DrawCommandBuffer : public CommandBuffer
{
private:
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

public:
    DrawCommandBuffer(Device const *device, CommandPool const *commandPool, VkCommandBuffer const &handle);
    DrawCommandBuffer(DrawCommandBuffer &&old);
    ~DrawCommandBuffer();

    VkSemaphore const &getImageAvailableSemaphore() const;
    VkSemaphore const &getRenderFinishedSemaphore() const;
    VkFence const &getInFlightFence() const;

    void waitForReady(Device const *device) const;
};
