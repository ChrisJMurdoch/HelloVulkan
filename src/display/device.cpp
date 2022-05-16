
#include "display/device.hpp"

#include "display/swapchain.hpp"
#include "utility/check.hpp"

Device::Device(PhysicalDevice const *physicalDevice, uint32_t graphicsQueueFamilyIndex, std::vector<const char*> const &validationLayers, std::vector<const char*> const &extensions)
{
    // Create array of queues (just combined main queue for now)
    float const queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{
        VkDeviceQueueCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = graphicsQueueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        }
    };

    // Create logical device
    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &deviceFeatures
    };
    check::fail( vkCreateDevice(physicalDevice->getHandle(), &createInfo, nullptr, &handle), "vkCreateDevice failed." );

    // Get generated queues
    vkGetDeviceQueue(handle, graphicsQueueFamilyIndex, 0, &mainQueue);
}

Device::~Device()
{
    vkDestroyDevice(handle, nullptr);
}

VkDevice const &Device::getHandle() const
{
    return handle;
}

Queue Device::getQueue() const
{
    return Queue(mainQueue);
}

Queue::Queue(VkQueue const &handle) : handle(handle)
{ }

VkQueue const &Queue::getHandle() const
{
    return handle;
}

void Queue::submit(Device const *device, VkSemaphore const &waitSemaphore, VkSemaphore const &signalSemaphore, VkFence const &fence, VkCommandBuffer const &commandBuffer)
{
    VkSemaphore waitSemaphores[] = {waitSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {signalSemaphore};
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };
    vkResetFences(device->getHandle(), 1, &fence);
    check::fail( vkQueueSubmit(handle, 1, &submitInfo, fence), "Failed to submit draw command buffer." );
}

void Queue::present(Swapchain const *swapchain, VkSemaphore const &waitSemaphore, uint32_t imageIndex)
{
    VkSwapchainKHR swapchains[] = {swapchain->getHandle()};
    VkSemaphore waitSemaphores[] = {waitSemaphore};
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex
    };
    vkQueuePresentKHR(handle, &presentInfo);
}
