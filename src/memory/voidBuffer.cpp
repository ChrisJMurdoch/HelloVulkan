
#include "memory/voidBuffer.hpp"

#include "configuration/physicalDevice.hpp"
#include "command/commandPool.hpp"
#include "command/commandBuffer.hpp"
#include "configuration/queue.hpp"
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

VoidBuffer::VoidBuffer
(
    Device const *device, PhysicalDevice const *physicalDevice, VkDeviceSize const &size,
    VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties
) : device(device), size(size)
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
}

VoidBuffer::VoidBuffer(VoidBuffer &&old) : device(old.device), size(old.size), handle(old.handle), memory(old.memory)
{
    old.handle = VK_NULL_HANDLE;
    old.memory = VK_NULL_HANDLE;
}

VoidBuffer::~VoidBuffer()
{
    vkDestroyBuffer(device->getHandle(), handle, nullptr);
    vkFreeMemory(device->getHandle(), memory, nullptr);
}

VkBuffer const &VoidBuffer::getHandle() const
{
    return handle;
}

uint32_t VoidBuffer::getOffset() const
{
    return 0; // Change later if necessary
}

void VoidBuffer::memcpy(size_t sourceDataSize, void const *sourceData)
{
    // Map device memory
    void *deviceData;
    vkMapMemory(device->getHandle(), memory, 0, sourceDataSize, 0, &deviceData);

    // Copy data
    std::memcpy(deviceData, sourceData, sourceDataSize);

    // Unmap device memory
    vkUnmapMemory(device->getHandle(), memory);
}

void VoidBuffer::transfer(CommandPool *commandPool, Queue queue, VoidBuffer const &sourceBuffer)
{
    // Create one-use command buffer
    CommandBuffer transferCommandBuffer = commandPool->allocateNewBuffer();

    // Record transfer command
    transferCommandBuffer.record([&](VkCommandBuffer const &commandBuffer)
    {
        VkBufferCopy copyRegion { .size = sourceBuffer.size };
        vkCmdCopyBuffer(commandBuffer, sourceBuffer.getHandle(), handle, 1, &copyRegion);
    }, true);

    // Submit command buffer to main queue
    queue.submit(device, transferCommandBuffer);

    // Wait for transfer to finish before freeing command buffer - TODO: batch transfers together to avoid multiple waits
    vkQueueWaitIdle(queue.getHandle());
}
