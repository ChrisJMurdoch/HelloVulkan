
#pragma once

#include "memory/buffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class Vertex;
class PhysicalDevice;

class VertexBuffer : public Buffer
{
private:
    int const nVertices;

public:
    VertexBuffer(Device const *device, std::vector<Vertex> const &vertices, PhysicalDevice const *physicalDevice);
    int getNVertices() const;
    uint32_t getOffset() const;
};
