
#include "memory/buffer.hpp"

#include "configuration/device.hpp"
#include "vertex/vertex.hpp"
#include "configuration/physicalDevice.hpp"
#include "utility/check.hpp"

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, PhysicalDevice const *physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice->getHandle(), &memoryProperties);

    for (int i=0; i<memoryProperties.memoryTypeCount; i++)
        if ( (typeFilter&(1<<i)) && ((memoryProperties.memoryTypes[i].propertyFlags&properties)==properties) )
            return i;

    throw std::exception("Failed to find suitable memory type.");
}

Buffer::Buffer
(
    Device const *device, void const *sourceData, PhysicalDevice const *physicalDevice,
    VkDeviceSize const &size, VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties
) : device(device)
{
    // Create buffer
    VkBufferCreateInfo bufferInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
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
        .memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties, physicalDevice)
    };
    check::fail( vkAllocateMemory(device->getHandle(), &allocInfo, nullptr, &memory), "vkAllocateMemory failed." );

    // Bind memory to buffer
    vkBindBufferMemory(device->getHandle(), handle, memory, 0);

    // Copy memory over
    void *data;
    vkMapMemory(device->getHandle(), memory, 0, bufferInfo.size, 0, &data);
    memcpy(data, sourceData, static_cast<size_t>(bufferInfo.size));
    vkUnmapMemory(device->getHandle(), memory);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(device->getHandle(), handle, nullptr);
    vkFreeMemory(device->getHandle(), memory, nullptr);
}

VkBuffer const &Buffer::getHandle() const
{
    return handle;
}
