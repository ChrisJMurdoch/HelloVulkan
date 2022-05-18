
#include "command/drawCommandBuffer.hpp"

#include "configuration/device.hpp"
#include "utility/check.hpp"

DrawCommandBuffer::DrawCommandBuffer(Device const *device, CommandPool const *commandPool, VkCommandBuffer const &handle) : CommandBuffer(device, commandPool, handle)
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

DrawCommandBuffer::DrawCommandBuffer(DrawCommandBuffer &&old) : CommandBuffer(std::move(old)),
    imageAvailableSemaphore(old.imageAvailableSemaphore), renderFinishedSemaphore(old.renderFinishedSemaphore), inFlightFence(old.inFlightFence)
{
    old.imageAvailableSemaphore = VK_NULL_HANDLE;
    old.renderFinishedSemaphore = VK_NULL_HANDLE;
    old.inFlightFence = VK_NULL_HANDLE;
}

DrawCommandBuffer::~DrawCommandBuffer()
{
    vkDestroySemaphore(device->getHandle(), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device->getHandle(), imageAvailableSemaphore, nullptr);
    vkDestroyFence(device->getHandle(), inFlightFence, nullptr);
}

VkSemaphore const &DrawCommandBuffer::getImageAvailableSemaphore() const
{
    return imageAvailableSemaphore;
}

VkSemaphore const &DrawCommandBuffer::getRenderFinishedSemaphore() const
{
    return renderFinishedSemaphore;
}

VkFence const &DrawCommandBuffer::getInFlightFence() const
{
    return inFlightFence;
}

void DrawCommandBuffer::waitForReady(Device const *device) const
{
    vkWaitForFences(device->getHandle(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
}
