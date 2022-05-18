
#include "memory/vertexBuffer.hpp"

#include "vertex/vertex.hpp"
#include "utility/check.hpp"

VertexBuffer::VertexBuffer(Device const *device, PhysicalDevice const *physicalDevice, std::vector<Vertex> const &vertices, VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties)
    : Buffer(
        device, physicalDevice, sizeof(vertices[0])*vertices.size(),
        usage, properties
    ), nVertices(vertices.size())
{ }

int VertexBuffer::getNVertices() const
{
    return nVertices;
}
