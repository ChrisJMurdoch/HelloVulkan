
#include "display/queue.hpp"

#include "display/device.hpp"
#include "display/swapchain.hpp"
#include "display/commandPool.hpp"
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
    std::vector<VkSemaphore> waitSemaphores = {commandBuffer.imageAvailableSemaphore};
    std::vector<VkPipelineStageFlags> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    std::vector<VkSemaphore> signalSemaphores = {commandBuffer.renderFinishedSemaphore};
    VkSubmitInfo submitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer.commandBuffer,
        .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphores = signalSemaphores.data()
    };
    vkResetFences(device->getHandle(), 1, &commandBuffer.inFlightFence);
    check::fail( vkQueueSubmit(handle, 1, &submitInfo, commandBuffer.inFlightFence), "vkQueueSubmit failed." );
}

void Queue::present(Swapchain const *swapchain, CommandBuffer const &commandBuffer, Image const &image)
{
    std::vector<VkSwapchainKHR> swapchains = {swapchain->getHandle()};
    std::vector<VkSemaphore> waitSemaphores = {commandBuffer.renderFinishedSemaphore};
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
