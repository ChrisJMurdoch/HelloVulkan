
#include "display/vertexBuffer.hpp"

#include "display/device.hpp"
#include "display/vertex.hpp"
#include "display/physicalDevice.hpp"
#include "utility/check.hpp"

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, PhysicalDevice const *physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice->getHandle(), &memoryProperties);

    for (int i=0; i<memoryProperties.memoryTypeCount; i++)
        if ( (typeFilter&(1 << i)) && ((memoryProperties.memoryTypes[i].propertyFlags&properties)==properties) )
            return i;

    throw std::runtime_error("Failed to find suitable memory type.");
}

VertexBuffer::VertexBuffer(Device const *device, std::vector<Vertex> const &vertices, PhysicalDevice const *physicalDevice) : device(device), nVertices(vertices.size())
{
    // Create buffer
    VkBufferCreateInfo bufferInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(vertices[0]) * vertices.size(),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    check::fail( vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, &handle), "vkCreateBuffer failed." );

    // Get memory requirements
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device->getHandle(), handle, &memoryRequirements);

    // Allocate memory
    VkMemoryAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, physicalDevice)
    };
    check::fail( vkAllocateMemory(device->getHandle(), &allocInfo, nullptr, &memory), "vkAllocateMemory failed." );

    // Bind memory to buffer
    vkBindBufferMemory(device->getHandle(), handle, memory, 0);

    // Copy memory over
    void *data;
    vkMapMemory(device->getHandle(), memory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferInfo.size));
    vkUnmapMemory(device->getHandle(), memory);
}

VertexBuffer::~VertexBuffer()
{
    vkDestroyBuffer(device->getHandle(), handle, nullptr);
    vkFreeMemory(device->getHandle(), memory, nullptr);
}

VkBuffer const &VertexBuffer::getHandle() const
{
    return handle;
}

int VertexBuffer::getNVertices() const
{
    return nVertices;
}

uint32_t VertexBuffer::getOffset() const
{
    return 0; // Change later if necessary
}
