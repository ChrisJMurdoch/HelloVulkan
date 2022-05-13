
#pragma once

#include <vulkan/vulkan.h>

class RenderPass
{
private:
    VkDevice const &device;
    VkRenderPass handle;

public:
    RenderPass(VkFormat const &format, VkDevice const &device);
    ~RenderPass();
    VkRenderPass const &getHandle() const;
};
