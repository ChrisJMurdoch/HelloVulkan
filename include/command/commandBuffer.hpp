
#pragma once

#include <vulkan/vulkan.h>

#include <functional>

class Device;
class Swapchain;

class CommandBuffer
{
private:
    VkCommandBuffer const &handle;

public:
    CommandBuffer(VkCommandBuffer const &handle);
    VkCommandBuffer const &getHandle() const;

    void record(std::function<void(VkCommandBuffer const &commandBuffer)> commands);
};
