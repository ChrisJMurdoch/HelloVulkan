
#include "command/commandBuffer.hpp"

#include "configuration/device.hpp"
#include "utility/check.hpp"

CommandBuffer::CommandBuffer(Device const *device, VkCommandBuffer const &handle) : device(device), handle(handle)
{
    // Create sync objects
    static VkSemaphoreCreateInfo semaphoreInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    static VkFenceCreateInfo fenceInfo
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    check::fail( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphore), "vkCreateSemaphore failed." );
    check::fail( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &renderFinishedSemaphore), "vkCreateSemaphore failed." );
    check::fail( vkCreateFence(device->getHandle(), &fenceInfo, nullptr, &inFlightFence), "vkCreateFence failed." );
}

CommandBuffer::CommandBuffer(CommandBuffer &&old) : device(old.device), handle(old.handle),
    imageAvailableSemaphore(old.imageAvailableSemaphore), renderFinishedSemaphore(old.renderFinishedSemaphore), inFlightFence(old.inFlightFence)
{
    old.imageAvailableSemaphore = VK_NULL_HANDLE;
    old.renderFinishedSemaphore = VK_NULL_HANDLE;
    old.inFlightFence = VK_NULL_HANDLE;
}

CommandBuffer::~CommandBuffer()
{
    vkDestroySemaphore(device->getHandle(), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device->getHandle(), imageAvailableSemaphore, nullptr);
    vkDestroyFence(device->getHandle(), inFlightFence, nullptr);
}

VkCommandBuffer const &CommandBuffer::getHandle() const
{
    return handle;
}

VkSemaphore const &CommandBuffer::getImageAvailableSemaphore() const
{
    return imageAvailableSemaphore;
}

VkSemaphore const &CommandBuffer::getRenderFinishedSemaphore() const
{
    return renderFinishedSemaphore;
}

VkFence const &CommandBuffer::getInFlightFence() const
{
    return inFlightFence;
}

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
