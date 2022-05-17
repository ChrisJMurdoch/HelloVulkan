
#include "display/display.hpp"

#include "display/window.hpp"
#include "display/instance.hpp"
#include "display/debugMessenger.hpp"
#include "display/surface.hpp"
#include "display/physicalDevice.hpp"
#include "display/device.hpp"
#include "display/swapchain.hpp"
#include "display/commandPool.hpp"
#include "display/image.hpp"
#include "display/queue.hpp"
#include "display/pipeline.hpp"
#include "display/renderPass.hpp"
#include "display/commandBuffer.hpp"

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
 *  Decrease coupling
 */

// CONSTANTS

std::vector<const char *> const VALIDATION_LAYERS{ "VK_LAYER_KHRONOS_validation" };
std::vector<const char *> const DEVICE_EXTENSIONS{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

// FUNCTIONS

Display::Display(int windowWidth, int windowHeight, char const *title, BufferingStrategy bufferingStrategy, bool enableValidationLayers)
{
    // Enable validations layers
    std::vector<const char *> activeValidationLayers = enableValidationLayers ? VALIDATION_LAYERS : std::vector<const char *>{};

    // GLFW
    window = new Window(windowWidth, windowHeight, title, framebufferResizeCallback, this);

    // Vulkan
    instance = new Instance(title, activeValidationLayers, DebugMessenger::debugMessengerCreateInfo);
    debugMessenger = new DebugMessenger(instance);
    surface = new Surface(instance, window);
    physicalDevice = new PhysicalDevice(instance, surface, DEVICE_EXTENSIONS);
    device = new Device(physicalDevice, activeValidationLayers, DEVICE_EXTENSIONS);
    swapchain = new Swapchain(device, physicalDevice, window, surface);
    commandPool = new CommandPool(device, physicalDevice->getGraphicsQueueFamilyIndex(), bufferingStrategy);
}

void Display::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    Display *display = reinterpret_cast<Display *>(glfwGetWindowUserPointer(window));
    display->framebufferResized = true;
}

Display::~Display()
{
    vkDeviceWaitIdle(device->getHandle());
    delete commandPool;
    delete swapchain;
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
        // Poll for GLFW input updates
        glfwPollEvents();

        // Render frame async
        drawFrame();

        // Display framerate
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
    // Get next command buffer and wait till ready
    CommandBuffer commandBuffer = commandPool->nextBuffer();
    commandBuffer.waitForReady(device);

    // Acquire valid image from swapchain
    Image image = swapchain->acquireNextImage(commandBuffer, framebufferResized, physicalDevice, window, surface);

    // Record commands into command buffer
    commandBuffer.record([&](VkCommandBuffer const &commandBuffer)
    {
        swapchain->getRenderPass()->run(swapchain, image, commandBuffer, [&]()
        {
            // Bind graphics pipeline with relevant shaders
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchain->getPipeline()->getHandle());

            // Draw triangles
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        });
    });

    // Submit command buffer to graphics/present queue
    device->getGraphicsQueue().submit(device, commandBuffer);
    
    // Present image
    device->getGraphicsQueue().present(swapchain, commandBuffer, image);
}
