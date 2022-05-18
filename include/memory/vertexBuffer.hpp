
#pragma once

#include "memory/buffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class PhysicalDevice;
class Vertex;

class VertexBuffer : public Buffer
{
private:
    int const nVertices;

public:
    VertexBuffer(Device const *device, PhysicalDevice const *physicalDevice, std::vector<Vertex> const &vertices, VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties);
    int getNVertices() const;
};
