
#include "command/commandBuffer.hpp"

#include "configuration/device.hpp"
#include "utility/check.hpp"

CommandBuffer::CommandBuffer(VkCommandBuffer const &handle) : handle(handle)
{ }

VkCommandBuffer const &CommandBuffer::getHandle() const
{
    return handle;
}

void CommandBuffer::record(std::function<void(VkCommandBuffer const &commandBuffer)> commands)
{
    // Reset buffer
    vkResetCommandBuffer(handle, 0);

    // Begin recording
    VkCommandBufferBeginInfo beginInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    check::fail( vkBeginCommandBuffer(handle, &beginInfo), "vkBeginCommandBuffer failed." );

    // Run commands to record
    commands(handle);

    // End recording
    check::fail( vkEndCommandBuffer(handle), "vkEndCommandBuffer failed." );
}
