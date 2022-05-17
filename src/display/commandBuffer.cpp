
#include "display/commandBuffer.hpp"

#include "display/device.hpp"
#include "utility/check.hpp"

CommandBuffer::CommandBuffer(VkCommandBuffer const &handle, VkSemaphore const &imageAvailableSemaphore, VkSemaphore const &renderFinishedSemaphore, VkFence const &inFlightFence)
    : handle{handle}, imageAvailableSemaphore{imageAvailableSemaphore}, renderFinishedSemaphore{renderFinishedSemaphore}, inFlightFence{inFlightFence}
{ }

void CommandBuffer::waitForReady(Device const *device) const
{
    vkWaitForFences(device->getHandle(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
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
