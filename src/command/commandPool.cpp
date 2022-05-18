
#include "command/commandPool.hpp"

#include "configuration/device.hpp"
#include "command/drawCommandBuffer.hpp"
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
    drawCommandBufferHandles.resize(maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) drawCommandBufferHandles.size()
    };
    check::fail( vkAllocateCommandBuffers(device->getHandle(), &allocInfo, drawCommandBufferHandles.data()), "vkAllocateCommandBuffers failed." );

    // Create synchronised command buffer objects
    drawCommandBuffers.reserve(drawCommandBufferHandles.size());
    for (VkCommandBuffer const &commandBufferHandle : drawCommandBufferHandles)
        drawCommandBuffers.push_back(DrawCommandBuffer(commandBufferHandle, device));
}

CommandPool::~CommandPool()
{
    drawCommandBuffers.clear();
    vkDestroyCommandPool(device->getHandle(), handle, nullptr);
}

VkCommandPool const &CommandPool::getHandle() const
{
    return handle;
}

DrawCommandBuffer &CommandPool::nextBuffer()
{
    static int index = 0;
    DrawCommandBuffer &commandBuffer = drawCommandBuffers[index];
    index++; index%=drawCommandBuffers.size();
    return commandBuffer;
}
