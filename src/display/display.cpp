
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
#include "display/vertex.hpp"
#include "display/vertexBuffer.hpp"

#include <chrono>

std::vector<const char *> const VALIDATION_LAYERS{ "VK_LAYER_KHRONOS_validation" };
std::vector<const char *> const DEVICE_EXTENSIONS{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

Display::Display(int windowWidth, int windowHeight, char const *title, BufferingStrategy bufferingStrategy, bool enableValidationLayers)
{
    // Optionally enable validations layers
    std::vector<const char *> activeValidationLayers = enableValidationLayers ? VALIDATION_LAYERS : std::vector<const char *>{};

    // Init GLFW
    window = new Window(windowWidth, windowHeight, title, framebufferResizeCallback, this);

    // Init Vulkan
    instance = new Instance(title, activeValidationLayers, DebugMessenger::debugMessengerCreateInfo);
    debugMessenger = new DebugMessenger(instance);
    surface = new Surface(instance, window);
    physicalDevice = new PhysicalDevice(instance, surface, DEVICE_EXTENSIONS);
    device = new Device(physicalDevice, activeValidationLayers, DEVICE_EXTENSIONS);
    swapchain = new Swapchain(device, physicalDevice, window, surface);
    commandPool = new CommandPool(device, physicalDevice->getMainQueueFamilyIndex(), bufferingStrategy);

    // Create vertex buffer
    std::vector<Vertex> const vertices
    {
        { {0.0f, -0.5f}, {1.0f, 1.0f, 1.0f} },
        { {0.5f, 0.5f},  {0.0f, 1.0f, 0.0f} },
        { {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} }
    };
    vertexBuffer = new VertexBuffer(device, vertices, physicalDevice);
}

void Display::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    Display *display = reinterpret_cast<Display *>(glfwGetWindowUserPointer(window));
    display->framebufferResized = true;
}

Display::~Display()
{
    // Wait until idle
    vkDeviceWaitIdle(device->getHandle());

    // Destroy vertex buffer
    delete vertexBuffer;

    // Destroy Vulkan objects
    delete commandPool;
    delete swapchain;
    delete device;
    delete physicalDevice;
    delete surface;
    delete debugMessenger;
    delete instance;

    // Destroy GLFW window
    delete window;
}

void Display::tick()
{
    // Record start time
    static int ticks = 0;
    static auto start = std::chrono::high_resolution_clock::now();

    // Poll for GLFW input updates
    glfwPollEvents();

    // Draw frame
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

bool Display::shouldClose() const
{
    return window->shouldClose();
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

            // Bind vertex buffers
            std::vector<VkBuffer> vertexBuffers { vertexBuffer->getHandle() };
            std::vector<VkDeviceSize> offsets   { vertexBuffer->getOffset() };
            vkCmdBindVertexBuffers(commandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), offsets.data());

            // Draw triangles
            vkCmdDraw(commandBuffer, vertexBuffer->getNVertices(), 1, 0, 0);
        });
    });

    // Submit command buffer to main queue
    device->getMainQueue().submit(device, commandBuffer);
    
    // Present image
    device->getMainQueue().present(swapchain, commandBuffer, image);
}
