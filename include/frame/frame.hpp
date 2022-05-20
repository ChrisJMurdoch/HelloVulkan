
#pragma once

#include "command/commandBuffer.hpp"

#include <vulkan/vulkan.h>

class Device;
class CommandPool;

/** Stores all per-frame-in-flight data necessary */
class Frame
{
private:
    Device const *device;
    CommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

public:
    Frame(Device const *device, CommandPool *commandPool);
    Frame(Frame &&old);
    ~Frame();
    
    CommandBuffer const &getCommandBuffer() const;
    CommandBuffer &getCommandBuffer();
    VkSemaphore const &getImageAvailableSemaphore() const;
    VkSemaphore const &getRenderFinishedSemaphore() const;
    VkFence const &getInFlightFence() const;
    
    void waitForReady(Device const *device) const;
};
