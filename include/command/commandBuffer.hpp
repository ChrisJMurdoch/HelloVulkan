
#pragma once

#include <vulkan/vulkan.h>

#include <functional>

class Device;
class CommandPool;

class CommandBuffer
{
protected:
    Device const *device;
    CommandPool const *commandPool;
    VkCommandBuffer handle;

public:
    CommandBuffer(Device const *device, CommandPool const *commandPool, VkCommandBuffer const &handle);
    CommandBuffer(CommandBuffer &&old);
    ~CommandBuffer();

    VkCommandBuffer const &getHandle() const;

    void record(std::function<void(VkCommandBuffer const &commandBuffer)> commands, bool singleUse=false);
};
