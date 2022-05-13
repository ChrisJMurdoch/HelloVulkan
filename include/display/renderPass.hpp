
#pragma once

#include <vulkan/vulkan.h>

class RenderPass
{
private:
    VkDevice const &device;
    VkRenderPass handle;

public:
    RenderPass(VkDevice const &device, VkFormat const &format);
    ~RenderPass();
    VkRenderPass const &getHandle() const;
};
