
#include "display/vertexBuffer.hpp"

#include "display/vertex.hpp"
#include "utility/check.hpp"

VertexBuffer::VertexBuffer(Device const *device, std::vector<Vertex> const &vertices, PhysicalDevice const *physicalDevice)
    : Buffer(
        device, vertices, physicalDevice,
        sizeof(vertices[0])*vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    ), nVertices(vertices.size())
{
}

int VertexBuffer::getNVertices() const
{
    return nVertices;
}

uint32_t VertexBuffer::getOffset() const
{
    return 0; // Change later if necessary
}
