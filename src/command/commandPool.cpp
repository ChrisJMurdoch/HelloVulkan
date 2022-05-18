
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
    commandBufferHandles.resize(maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) commandBufferHandles.size()
    };
    check::fail( vkAllocateCommandBuffers(device->getHandle(), &allocInfo, commandBufferHandles.data()), "vkAllocateCommandBuffers failed." );

    // Create synchronised command buffer objects
    commandBuffers.reserve(commandBufferHandles.size());
    for (VkCommandBuffer const &commandBufferHandle : commandBufferHandles)
        commandBuffers.push_back(CommandBuffer(device, commandBufferHandle));
}

CommandPool::~CommandPool()
{
    commandBuffers.clear();
    vkDestroyCommandPool(device->getHandle(), handle, nullptr);
}

VkCommandPool const &CommandPool::getHandle() const
{
    return handle;
}

CommandBuffer &CommandPool::nextBuffer()
{
    static int index = 0;
    CommandBuffer &commandBuffer = commandBuffers[index];
    index++; index%=commandBuffers.size();
    return commandBuffer;
}
