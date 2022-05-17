
#include "display/vertex.hpp"

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
    return VkVertexInputBindingDescription
    {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
}

std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions()
{
    return std::vector<VkVertexInputAttributeDescription>
    {
        VkVertexInputAttributeDescription
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, position)
        },
        VkVertexInputAttributeDescription
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, colour)
        }
    };
}

Vertex::Vertex(glm::vec2 position, glm::vec3 colour) : position(position), colour(colour)
{
}
