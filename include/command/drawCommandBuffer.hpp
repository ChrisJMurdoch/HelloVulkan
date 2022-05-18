
#pragma once

#include "command/commandBuffer.hpp"

#include <vulkan/vulkan.h>

#include <functional>

class Device;

class DrawCommandBuffer : public CommandBuffer
{
private:
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    Device const *device;

public:
    DrawCommandBuffer(VkCommandBuffer const &handle, Device const *device);
    DrawCommandBuffer(DrawCommandBuffer &&old);
    ~DrawCommandBuffer();

    VkSemaphore const &getImageAvailableSemaphore() const;
    VkSemaphore const &getRenderFinishedSemaphore() const;
    VkFence const &getInFlightFence() const;

    void waitForReady(Device const *device) const;
};
