
#include "display/display.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "display/choose.hpp"
#include "display/shaderModule.hpp"
#include "utility/io.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
#include <bitset>

/** 
 *  TODO
 * 
 *  Change Device::getHandle() args to Device
 *  Ensure all new-delete and vkCreate-vkDestroy are complete
 *  Move swapchain recreation from function call to full new-delete calls
 *  Fix class function visibility
 */

// PARAMETERS

const int MAX_FRAMES_IN_FLIGHT = 2;
std::vector<const char *> const validationLayers{ "VK_LAYER_KHRONOS_validation" };
std::vector<const char *> const deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

// Inline error checking
inline void nullThrow (void *value,    char const *message = "Error: null value")         { if (value == nullptr)    throw std::exception(message); }
inline void failThrow (VkResult value, char const *message = "Error: vulkan call failed") { if (value != VK_SUCCESS) throw std::exception(message); }

// PUBLIC FUNCTIONS

Display::Display(int windowWidth, int windowHeight)
{
    window = new Window(windowWidth, windowHeight, framebufferResizeCallback, this);
    initVulkan();
}

void Display::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    Display *display = reinterpret_cast<Display *>(glfwGetWindowUserPointer(window));
    display->framebufferResized = true;
}

void Display::run() {
    mainLoop();
    cleanup();
}

void Display::initVulkan()
{
    instance = new Instance(DebugMessenger::debugMessengerCreateInfo, validationLayers);
    debugMessenger = new DebugMessenger(instance);
    surface = new Surface(instance, window);
    physicalDevice = new PhysicalDevice(device->getHandle(), instance, surface->getHandle(), deviceExtensions);
    graphicsQueueFamilyIndex = PhysicalDevice::getGraphicsQueueFamilyIndex(physicalDevice->getHandle(), surface->getHandle());
    device = new Device(physicalDevice, graphicsQueueFamilyIndex, validationLayers, deviceExtensions);
    swapChain = new Swapchain(device, physicalDevice, window, surface);
    commandPool = new CommandPool(device->getHandle(), graphicsQueueFamilyIndex, MAX_FRAMES_IN_FLIGHT);
    createSyncObjects();
}

void Display::mainLoop()
{
    while (!window->shouldClose())
    {
        glfwPollEvents();
        drawFrame();
    }
}

void Display::cleanup()
{
    vkDeviceWaitIdle(device->getHandle());

    delete swapChain;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device->getHandle(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device->getHandle(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device->getHandle(), inFlightFences[i], nullptr);
    }

    delete commandPool;

    delete device;

    delete debugMessenger;

    delete physicalDevice;

    delete surface;
    
    delete instance;

    delete window;
}

void Display::createSyncObjects()
{
    // One set of mutexes for each frame
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    // Mutex creation info
    VkSemaphoreCreateInfo semaphoreInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    VkFenceCreateInfo fenceInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    // Create mutexes for each frame
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        failThrow( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]), "Failed to create semaphore for a frame." );
        failThrow( vkCreateSemaphore(device->getHandle(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]), "Failed to create semaphore for a frame." );
        failThrow( vkCreateFence(device->getHandle(), &fenceInfo, nullptr, &inFlightFences[i]),                   "Failed to create fence for a frame."     );
    }
}

void Display::drawFrame()
{
    // Wait for current frame to become available
    vkWaitForFences(device->getHandle(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire valid image
    uint32_t imageIndex;
    VkResult result;
    while (true)
    {
        // Acquire image
        result = vkAcquireNextImageKHR(device->getHandle(), swapChain->getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        // If necessary, regenerate and repeat
        if ( result!=VK_SUCCESS || framebufferResized )
        {
            framebufferResized = false;
            swapChain->recreateSwapChain(physicalDevice, window, surface);
            std::cout << "Swapchain regenerated." << std::endl;
        }
        else
        {
            break;
        }
    }

    // Record commands
    commandPool->record(swapChain, currentFrame, imageIndex, [&](VkCommandBuffer const &commandBuffer)
    {
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = swapChain->getRenderPass()->getHandle(),
            .framebuffer = swapChain->getFramebuffers()[imageIndex],
            .renderArea{
                .offset = {0, 0},
                .extent = swapChain->getExtent()
            },
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, swapChain->getPipeline()->getHandle());

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
    });

    // Submit queue
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandPool->getBuffers()[currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };
    vkResetFences(device->getHandle(), 1, &inFlightFences[currentFrame]);
    failThrow( vkQueueSubmit(device->getQueue(), 1, &submitInfo, inFlightFences[currentFrame]), "Failed to submit draw command buffer." );

    // Present queue
    VkSwapchainKHR swapChains[] = {swapChain->getHandle()};
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex
    };
    result = vkQueuePresentKHR(device->getQueue(), &presentInfo);

    // Change frame index to next
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
