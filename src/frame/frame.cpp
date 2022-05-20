
#include "frame/frame.hpp"

#include "configuration/device.hpp"
#include "command/commandPool.hpp"
#include "configuration/physicalDevice.hpp"
#include "utility/check.hpp"

Frame::Frame(Device const *device, CommandPool *commandPool, PhysicalDevice const *physicalDevice)
  : device(device),
    commandBuffer(commandPool->allocateNewBuffer()),
    uniformObjectBuffer(device, physicalDevice, sizeof(UniformObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
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

Frame::Frame(Frame &&old)
  : device(old.device),
    commandBuffer(std::move(old.commandBuffer)),
    imageAvailableSemaphore(old.imageAvailableSemaphore),
    renderFinishedSemaphore(old.renderFinishedSemaphore),
    inFlightFence(old.inFlightFence),
    uniformObjectBuffer(std::move(old.uniformObjectBuffer))
{
    old.imageAvailableSemaphore = VK_NULL_HANDLE;
    old.renderFinishedSemaphore = VK_NULL_HANDLE;
    old.inFlightFence = VK_NULL_HANDLE;
}

Frame::~Frame()
{
    vkDestroySemaphore(device->getHandle(), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device->getHandle(), imageAvailableSemaphore, nullptr);
    vkDestroyFence(device->getHandle(), inFlightFence, nullptr);
}

CommandBuffer const &Frame::getCommandBuffer() const
{
    return commandBuffer;
}

CommandBuffer &Frame::getCommandBuffer()
{
    return commandBuffer;
}

VkSemaphore const &Frame::getImageAvailableSemaphore() const
{
    return imageAvailableSemaphore;
}

VkSemaphore const &Frame::getRenderFinishedSemaphore() const
{
    return renderFinishedSemaphore;
}

VkFence const &Frame::getInFlightFence() const
{
    return inFlightFence;
}

void Frame::waitForReady(Device const *device) const
{
    vkWaitForFences(device->getHandle(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
}

void Frame::updateUniform(UniformObject const &uniform)
{
    std::vector<UniformObject> data { uniform };
    uniformObjectBuffer.memcpy(data);
}
