
#pragma once

#include <vulkan/vulkan.h>

#include <functional>

class Device;
class Swapchain;
class Image;

class RenderPass
{
private:
    Device const *device;
    VkRenderPass handle;

public:
    RenderPass(Device const *device, VkFormat const &format);
    ~RenderPass();
    VkRenderPass const &getHandle() const;

    void run(Swapchain const *swapchain, Image const &image, VkCommandBuffer const &commandBuffer, std::function<void()> commands);
};
