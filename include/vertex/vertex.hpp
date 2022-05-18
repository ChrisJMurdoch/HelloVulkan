
#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vector>

class Vertex
{
public:
    glm::vec2 position;
    glm::vec3 colour;

public:
    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

public:
    Vertex(glm::vec2 position, glm::vec3 colour);
};
