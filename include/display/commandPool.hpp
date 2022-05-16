
#pragma once

#include "display/swapchain.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <functional>

/** Also contains relevant synchronisation objects for command buffer */
class CommandBuffer
{
public:
    VkCommandBuffer &commandBuffer;
    VkSemaphore &imageAvailableSemaphore;
    VkSemaphore &renderFinishedSemaphore;
    VkFence& inFlightFence;
    CommandBuffer(VkCommandBuffer &commandBuffer, VkSemaphore &imageAvailableSemaphore, VkSemaphore &renderFinishedSemaphore, VkFence& inFlightFence);
    void waitForReady(Device const *device) const;
};

class CommandPool
{
private:
    Device const *device;
    VkCommandPool handle;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

public:
    CommandPool(Device const *device, uint32_t const graphicsQueueFamilyIndex, int maxFramesInFlight);
    ~CommandPool();
    VkCommandPool const &getHandle() const;
    CommandBuffer getBuffer(int index);
    void record(Swapchain const *swapchain, int commandBufferIndex, int frameBufferIndex, std::function<void(VkCommandBuffer const &commandBuffer)> commands);
};
