
#pragma once

#include <vulkan/vulkan.h>

class Device;
class Swapchain;
class DrawCommandBuffer;
class Image;

class Queue
{
private:
    friend class Device;
    VkQueue const &handle;

private:
    Queue(VkQueue const &handle);

public:
    VkQueue const &getHandle() const;
    void submit(Device const *device, DrawCommandBuffer const &commandBuffer);
    void present(Swapchain const *swapchain, DrawCommandBuffer const &commandBuffer, Image const &image);
};
