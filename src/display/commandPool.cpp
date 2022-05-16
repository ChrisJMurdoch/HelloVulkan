
#include "display/commandPool.hpp"

#include "utility/check.hpp"

CommandBuffer::CommandBuffer(VkCommandBuffer &commandBuffer, VkSemaphore &imageAvailableSemaphore, VkSemaphore &renderFinishedSemaphore, VkFence& inFlightFence)
    : commandBuffer{commandBuffer}, imageAvailableSemaphore{imageAvailableSemaphore}, renderFinishedSemaphore{renderFinishedSemaphore}, inFlightFence{inFlightFence}
{ }

void CommandBuffer::waitForReady(Device const *device) const
{
    vkWaitForFences(device->getHandle(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
}

CommandPool::CommandPool(Device const *device, uint32_t const graphicsQueueFamilyIndex, int maxFramesInFlight) : device(device)
{
    // Create CommandPool
    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphicsQueueFamilyIndex
    };
    check::fail( vkCreateCommandPool(device->getHandle(), &poolInfo, nullptr, &handle), "vkCreateCommandPool failed." );

    // Allocate CommandBuffers
    commandBuffers.resize(maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) commandBuffers.size()
    };
    check::fail( vkAllocateCommandBuffers(device->getHandle(), &allocInfo, commandBuffers.data()), "vkAllocateCommandBuffers failed." );

    // Create sync objects

    // One set of mutexes for each frame
    imageAvailableSemaphores.resize(maxFramesInFlight);
    renderFinishedSemaphores.resize(maxFramesInFlight);
    inFlightFences.resize(maxFramesInFlight);

    // Mutex creation info
    VkSemaphoreCreateInfo semaphoreInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    VkFenceCreateInfo fenceInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    // Create mutexes for each frame
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        check::fail( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]), "Failed to create semaphore for a frame." );
        check::fail( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]), "Failed to create semaphore for a frame." );
        check::fail( vkCreateFence(device->getHandle(), &fenceInfo, nullptr, &inFlightFences[i]),                   "Failed to create fence for a frame."     );
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

CommandBuffer CommandPool::getBuffer(int index)
{
    return CommandBuffer(commandBuffers[index], imageAvailableSemaphores[index], renderFinishedSemaphores[index], inFlightFences[index]);
}

void CommandPool::record(Swapchain const *swapchain, int commandBufferIndex, int frameBufferIndex, std::function<void(VkCommandBuffer const &commandBuffer)> commands)
{
    // Get active buffer
    VkCommandBuffer commandBuffer = commandBuffers[commandBufferIndex];

    // Reset buffer
    vkResetCommandBuffer(commandBuffer, 0);

    // Begin recording
    VkCommandBufferBeginInfo beginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    check::fail( vkBeginCommandBuffer(commandBuffer, &beginInfo), "vkBeginCommandBuffer failed." );

    // Run commands to record
    commands(commandBuffer);

    // End recording
    check::fail( vkEndCommandBuffer(commandBuffer), "vkEndCommandBuffer failed." );
}
