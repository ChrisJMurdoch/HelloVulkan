
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class CommandBuffer;
class DrawCommandBuffer;

class CommandPool
{
private:
    Device const *device;
    VkCommandPool handle;
    std::vector<DrawCommandBuffer> drawCommandBuffers;

public:
    CommandPool(Device const *device, uint32_t const mainQueueFamilyIndex, int maxFramesInFlight);
    ~CommandPool();
    VkCommandPool const &getHandle() const;
    DrawCommandBuffer &nextBuffer();
    CommandBuffer allocateNewBuffer();
};
