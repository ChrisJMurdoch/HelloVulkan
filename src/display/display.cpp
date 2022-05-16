
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
#include <chrono>

/** 
 *  TODO
 * 
 *  Change Device::getHandle() args to Device
 *  Ensure all new-delete and vkCreate-vkDestroy are complete
 *  Move swapchain recreation from function call to full new-delete calls
 *  Fix class function visibility
 *  Create classes to encapsulate:
 *   - Framebuffers in swapchain
 *   - Frames in flight
 *  Change brace inits to strict
 *  Use forward headers
 *  Clean up main includes
 */

// PARAMETERS

const int MAX_FRAMES_IN_FLIGHT = 3;
std::vector<const char *> const validationLayers{ "VK_LAYER_KHRONOS_validation" };
std::vector<const char *> const deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

// FUNCTIONS

Display::Display(int windowWidth, int windowHeight)
{
    // GLFW
    window = new Window(windowWidth, windowHeight, framebufferResizeCallback, this);

    // Vulkan
    instance = new Instance(DebugMessenger::debugMessengerCreateInfo, validationLayers);
    debugMessenger = new DebugMessenger(instance);
    surface = new Surface(instance, window);
    physicalDevice = new PhysicalDevice(device->getHandle(), instance, surface->getHandle(), deviceExtensions);
    graphicsQueueFamilyIndex = PhysicalDevice::getGraphicsQueueFamilyIndex(physicalDevice->getHandle(), surface->getHandle());
    device = new Device(physicalDevice, graphicsQueueFamilyIndex, validationLayers, deviceExtensions);
    swapChain = new Swapchain(device, physicalDevice, window, surface);
    commandPool = new CommandPool(device, graphicsQueueFamilyIndex, MAX_FRAMES_IN_FLIGHT);
}

void Display::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    Display *display = reinterpret_cast<Display *>(glfwGetWindowUserPointer(window));
    display->framebufferResized = true;
}

Display::~Display()
{
    vkDeviceWaitIdle(device->getHandle());
    delete commandPool;
    delete swapChain;
    delete device;
    delete physicalDevice;
    delete surface;
    delete debugMessenger;
    delete instance;
    delete window;
}

void Display::run()
{
    // Record start time
    int ticks = 0;
    auto start = std::chrono::high_resolution_clock::now();

    // Main loop
    while (!window->shouldClose())
    {
        glfwPollEvents();
        drawFrame();

        // Check framerate
        ticks++;
        if (ticks%5000==0)
        {
            auto now = std::chrono::high_resolution_clock::now();
            long long nano = std::chrono::duration_cast<std::chrono::nanoseconds>(now-start).count();
            std::cout << "Framerate: " << std::floor(ticks/(nano/1000000000.0)) << "Hz." << std::endl;
        }
    }
}

void Display::drawFrame()
{
    // Get current command buffer
    static int currentFrame = 0;
    CommandBuffer commandBuffer = commandPool->getBuffer(currentFrame);

    // Wait for current frame to become available
    vkWaitForFences(device->getHandle(), 1, &commandBuffer.inFlightFence, VK_TRUE, UINT64_MAX);

    // Acquire valid image
    uint32_t imageIndex;
    VkResult result;
    while (true)
    {
        // Acquire image
        result = vkAcquireNextImageKHR(device->getHandle(), swapChain->getHandle(), UINT64_MAX, commandBuffer.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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
        swapChain->getRenderPass()->record(swapChain, imageIndex, commandBuffer, [&]()
        {
            // Bind graphics pipeline with specific shaders
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, swapChain->getPipeline()->getHandle());

            // Draw triangles
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        });
    });

    device->getQueue().submit(device, commandBuffer.imageAvailableSemaphore, commandBuffer.renderFinishedSemaphore, commandBuffer.inFlightFence, commandPool->getBuffer(currentFrame));
    
    device->getQueue().present(swapChain, commandBuffer.renderFinishedSemaphore, imageIndex);

    // Change frame index to next
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
