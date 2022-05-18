
#pragma once

#include <vulkan/vulkan.h>

#include <functional>

class Device;
class Swapchain;

class CommandBuffer
{
public:
    VkCommandBuffer const &handle;
    VkSemaphore const &imageAvailableSemaphore;
    VkSemaphore const &renderFinishedSemaphore;
    VkFence const &inFlightFence;

public:
    CommandBuffer(VkCommandBuffer const &handle, VkSemaphore const &imageAvailableSemaphore, VkSemaphore const &renderFinishedSemaphore, VkFence const &inFlightFence);

    void waitForReady(Device const *device) const;
    void record(std::function<void(VkCommandBuffer const &commandBuffer)> commands);
};
