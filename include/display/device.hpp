
#pragma once

#include "display/physicalDevice.hpp"

#include <vulkan/vulkan.h>

class Swapchain;
class CommandBuffer;

class Queue
{
private:
    VkQueue const &handle;

private:
    friend class Device;
    Queue(VkQueue const &handle);

public:
    VkQueue const &getHandle() const;
    void submit(Device const *device, VkSemaphore const &waitSemaphore, VkSemaphore const &signalSemaphore, VkFence const &fence, CommandBuffer const &commandBuffer);
    void present(Swapchain const *swapchain, VkSemaphore const &waitSemaphore, uint32_t imageIndex);
};

class Device
{
private:
    VkDevice handle;
    VkQueue mainQueue;

public:
    Device(PhysicalDevice const *physicalDevice, uint32_t graphicsQueueFamilyIndex, std::vector<const char*> const &validationLayers, std::vector<const char*> const &extensions);
    ~Device();
    VkDevice const &getHandle() const;
    Queue getQueue() const;
};
