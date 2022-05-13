
#include "display/commandPool.hpp"

#include "utility/check.hpp"

CommandPool::CommandPool(VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, int maxFramesInFlight) : device(device)
{
    // Create CommandPool
    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
    };
    check::fail( vkCreateCommandPool(device, &poolInfo, nullptr, &handle), "vkCreateCommandPool failed." );

    // Allocate CommandBuffers
    commandBuffers.resize(maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) commandBuffers.size()
    };
    check::fail( vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()), "vkAllocateCommandBuffers failed." );
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device, handle, nullptr);
}

VkCommandPool const &CommandPool::getHandle() const
{
    return handle;
}

std::vector<VkCommandBuffer> const &CommandPool::getBuffers() const
{
    return commandBuffers;
}
