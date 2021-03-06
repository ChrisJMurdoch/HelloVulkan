
#include "command/commandPool.hpp"

#include "configuration/device.hpp"
#include "command/commandBuffer.hpp"
#include "utility/check.hpp"

CommandPool::CommandPool(Device const *device, uint32_t const mainQueueFamilyIndex, int maxFramesInFlight) : device(device)
{
    // Create CommandPool
    VkCommandPoolCreateInfo poolInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = mainQueueFamilyIndex
    };
    check::fail( vkCreateCommandPool(device->getHandle(), &poolInfo, nullptr, &handle), "vkCreateCommandPool failed." );

    // Allocate command buffers
    std::vector<VkCommandBuffer> drawCommandBufferHandles(maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) drawCommandBufferHandles.size()
    };
    check::fail( vkAllocateCommandBuffers(device->getHandle(), &allocInfo, drawCommandBufferHandles.data()), "vkAllocateCommandBuffers failed." );
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device->getHandle(), handle, nullptr);
}

VkCommandPool const &CommandPool::getHandle() const
{
    return handle;
}

CommandBuffer CommandPool::allocateNewBuffer()
{
    // Allocate command buffers
    VkCommandBuffer commandBufferHandle;
    VkCommandBufferAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    check::fail( vkAllocateCommandBuffers(device->getHandle(), &allocInfo, &commandBufferHandle), "vkAllocateCommandBuffers failed." );

    // Create synchronised command buffer objects
    return CommandBuffer(device, this, commandBufferHandle);
}
