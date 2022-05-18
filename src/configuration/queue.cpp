
#include "configuration/queue.hpp"

#include "configuration/device.hpp"
#include "swapchain/swapchain.hpp"
#include "command/commandPool.hpp"
#include "swapchain/image.hpp"
#include "command/drawCommandBuffer.hpp"
#include "command/commandBuffer.hpp"
#include "utility/check.hpp"

#include <vector>

Queue::Queue(VkQueue const &handle) : handle(handle)
{ }

VkQueue const &Queue::getHandle() const
{
    return handle;
}

void Queue::submit(Device const *device, CommandBuffer const &commandBuffer)
{
    VkSubmitInfo submitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer.getHandle()
    };
    check::fail( vkQueueSubmit(handle, 1, &submitInfo, VK_NULL_HANDLE), "vkQueueSubmit failed." );
}

void Queue::drawSubmit(Device const *device, DrawCommandBuffer const &commandBuffer)
{
    std::vector<VkSemaphore> waitSemaphores = {commandBuffer.getImageAvailableSemaphore()};
    std::vector<VkPipelineStageFlags> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    std::vector<VkSemaphore> signalSemaphores = {commandBuffer.getRenderFinishedSemaphore()};
    VkSubmitInfo submitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer.getHandle(),
        .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphores = signalSemaphores.data()
    };
    vkResetFences(device->getHandle(), 1, &commandBuffer.getInFlightFence());
    check::fail( vkQueueSubmit(handle, 1, &submitInfo, commandBuffer.getInFlightFence()), "vkQueueSubmit failed." );
}

void Queue::present(Swapchain const *swapchain, DrawCommandBuffer const &commandBuffer, Image const &image)
{
    std::vector<VkSwapchainKHR> swapchains = {swapchain->getHandle()};
    std::vector<VkSemaphore> waitSemaphores = {commandBuffer.getRenderFinishedSemaphore()};
    VkPresentInfoKHR presentInfo
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .swapchainCount = static_cast<uint32_t>(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = &image.index
    };
    check::fail( vkQueuePresentKHR(handle, &presentInfo), "vkQueuePresentKHR failed." );
}
