
#pragma once

#include "configuration/device.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class Vertex;
class PhysicalDevice;
class CommandPool;
class Queue;

class VoidBuffer
{
private:
    Device const *device;
    VkBuffer handle;
    VkDeviceMemory memory;

protected:
    VkDeviceSize const size;

protected:
    VoidBuffer
    (
        Device const *device, PhysicalDevice const *physicalDevice, VkDeviceSize const &size,
        VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties
    );

public:
    VoidBuffer(VoidBuffer &&old);
    ~VoidBuffer();

    VkBuffer const &getHandle() const;
    uint32_t getOffset() const;
    VkDeviceSize getSize() const;
    void memcpy(size_t sourceDataSize, void const *sourceData);
    void transfer(CommandPool *commandPool, Queue queue, VoidBuffer const &sourceBuffer);
};
