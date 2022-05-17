
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class Vertex;
class PhysicalDevice;

class VertexBuffer
{
private:
    VkBuffer handle;
    VkDeviceMemory memory;
    Device const *device;
    int const nVertices;

public:
    VertexBuffer(Device const *device, std::vector<Vertex> const &vertices, PhysicalDevice const *physicalDevice);
    ~VertexBuffer();
    VkBuffer const &getHandle() const;
    int getNVertices() const;
    uint32_t getOffset() const;
};
