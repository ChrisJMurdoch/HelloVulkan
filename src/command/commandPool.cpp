
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
    commandBuffers.resize(maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) commandBuffers.size()
    };
    check::fail( vkAllocateCommandBuffers(device->getHandle(), &allocInfo, commandBuffers.data()), "vkAllocateCommandBuffers failed." );

    // One set of mutexes for each frame in flight (command buffer)
    imageAvailableSemaphores.resize(maxFramesInFlight);
    renderFinishedSemaphores.resize(maxFramesInFlight);
    inFlightFences.resize(maxFramesInFlight);

    // Mutex creation info
    VkSemaphoreCreateInfo semaphoreInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    VkFenceCreateInfo fenceInfo
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    // Create mutexes for each frame in flight (command buffer)
    for (int i=0; i<maxFramesInFlight; i++)
    {
        check::fail( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]), "vkCreateSemaphore failed." );
        check::fail( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]), "vkCreateSemaphore failed." );
        check::fail( vkCreateFence(device->getHandle(), &fenceInfo, nullptr, &inFlightFences[i]), "vkCreateFence failed." );
    }
}

CommandPool::~CommandPool()
{
    for (int i=0; i<renderFinishedSemaphores.size(); i++)
    {
        vkDestroySemaphore(device->getHandle(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device->getHandle(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device->getHandle(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device->getHandle(), handle, nullptr);
}

VkCommandPool const &CommandPool::getHandle() const
{
    return handle;
}

CommandBuffer CommandPool::nextBuffer()
{
    static int index = 0;
    return CommandBuffer(commandBuffers[index], imageAvailableSemaphores[index], renderFinishedSemaphores[index], inFlightFences[index]);
    index++;
    index %= commandBuffers.size();
}
