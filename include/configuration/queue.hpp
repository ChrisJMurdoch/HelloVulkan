
#pragma once

#include <vulkan/vulkan.h>

class Device;
class Swapchain;
class CommandBuffer;
class Frame;
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
    void submit(Device const *device, CommandBuffer const &commandBuffer);
    void drawSubmit(Device const *device, Frame const &frame);
    void present(Swapchain const *swapchain, Frame const &frame, Image const &image);
};
