
#pragma once

#include <vulkan/vulkan.h>

#include <functional>

class Swapchain;
class Image;

class RenderPass
{
private:
    VkDevice const &device;
    VkRenderPass handle;

public:
    RenderPass(VkDevice const &device, VkFormat const &format);
    ~RenderPass();
    VkRenderPass const &getHandle() const;

    void record(Swapchain const *swapchain, Image const &image, VkCommandBuffer const &commandBuffer, std::function<void()> commands) const;
};
