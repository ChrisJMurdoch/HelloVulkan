
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class Vertex;
class PhysicalDevice;

class Buffer
{
private:
    VkBuffer handle;
    VkDeviceMemory memory;
    Device const *device;

protected:
    Buffer
    (
        Device const *device, std::vector<Vertex> const &vertices, PhysicalDevice const *physicalDevice,
        VkDeviceSize const &size, VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties
    );

public:
    ~Buffer();
    VkBuffer const &getHandle() const;
};
