
#include "command/commandBuffer.hpp"

#include "configuration/device.hpp"
#include "command/commandPool.hpp"
#include "utility/check.hpp"

CommandBuffer::CommandBuffer(Device const *device, CommandPool const *commandPool, VkCommandBuffer const &handle) : device(device), commandPool(commandPool), handle(handle)
{ }

CommandBuffer::CommandBuffer(CommandBuffer &&old) : device(old.device), commandPool(old.commandPool), handle(old.handle)
{
    old.handle = VK_NULL_HANDLE;
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(device->getHandle(), commandPool->getHandle(), 1, &handle);
}

VkCommandBuffer const &CommandBuffer::getHandle() const
{
    return handle;
}

void CommandBuffer::record(std::function<void(VkCommandBuffer const &commandBuffer)> commands, bool singleUse)
{
    // Reset buffer
    vkResetCommandBuffer(handle, 0);

    // Begin recording
    VkCommandBufferBeginInfo beginInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    if (singleUse)
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    check::fail( vkBeginCommandBuffer(handle, &beginInfo), "vkBeginCommandBuffer failed." );

    // Run commands to record
    commands(handle);

    // End recording
    check::fail( vkEndCommandBuffer(handle), "vkEndCommandBuffer failed." );
}
