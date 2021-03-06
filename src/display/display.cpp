
#include "display/display.hpp"

#include "configuration/window.hpp"
#include "configuration/instance.hpp"
#include "configuration/debugMessenger.hpp"
#include "configuration/surface.hpp"
#include "configuration/physicalDevice.hpp"
#include "configuration/device.hpp"
#include "swapchain/swapchain.hpp"
#include "command/commandPool.hpp"
#include "swapchain/image.hpp"
#include "configuration/queue.hpp"
#include "swapchain/pipeline.hpp"
#include "swapchain/renderPass.hpp"
#include "vertex/vertex.hpp"
#include "memory/typedBuffer.hpp"
#include "frame/framePool.hpp"
#include "frame/frame.hpp"
#include "memory/descriptorSetLayout.hpp"
#include "utility/util.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    descriptorSetLayout = new DescriptorSetLayout(device);
    swapchain = new Swapchain(device, physicalDevice, window, surface, descriptorSetLayout);
    commandPool = new CommandPool(device, physicalDevice->getMainQueueFamilyIndex(), bufferingStrategy);
    framePool = new FramePool(device, commandPool, physicalDevice, bufferingStrategy, descriptorSetLayout);

    // Create vertices
    const std::vector<Vertex> vertices
    {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    // Create staging buffer and copy over data
    TypedBuffer<Vertex> vertexStagingBuffer(device, physicalDevice, util::vecsizeof(vertices), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexStagingBuffer.memcpy(vertices);

    // Create vertex buffer and transfer data
    vertexBuffer = new TypedBuffer<Vertex>(device, physicalDevice, util::vecsizeof(vertices), VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vertexBuffer->transfer(commandPool, device->getMainQueue(), vertexStagingBuffer);

    // Create indices
    const std::vector<uint16_t> indices
    {
        0, 1, 2,
        2, 3, 0
    };
    
    // Create staging buffer and copy over data
    TypedBuffer<uint16_t> indexStagingBuffer(device, physicalDevice, util::vecsizeof(indices), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    indexStagingBuffer.memcpy(indices);

    // Create index buffer and transfer data
    indexBuffer = new TypedBuffer<uint16_t>(device, physicalDevice, util::vecsizeof(indices), VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    indexBuffer->transfer(commandPool, device->getMainQueue(), indexStagingBuffer);
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

    // Destroy buffer
    delete indexBuffer;
    delete vertexBuffer;

    // Destroy Vulkan objects
    delete framePool;
    delete commandPool;
    delete swapchain;
    delete descriptorSetLayout;
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
    // Get next frame and wait till ready
    Frame &frame = framePool->nextFrame();
    frame.waitForReady(device);

    // Update uniforms
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    UniformObject uniform
    {
        .model = glm::rotate(glm::mat4(1.0f), deltaTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        .view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        .proj = glm::perspective(glm::radians(45.0f), swapchain->getExtent().width / (float) swapchain->getExtent().height, 0.1f, 10.0f)
    };
    uniform.proj[1][1] *= -1;
    frame.updateUniform(uniform);

    // Acquire valid image from swapchain
    Image image = swapchain->acquireNextImage(frame, framebufferResized, physicalDevice, window, surface, descriptorSetLayout);

    // Record commands into command buffer
    frame.getCommandBuffer().record([&](VkCommandBuffer const &commandBuffer)
    {
        swapchain->getRenderPass()->run(swapchain, image, commandBuffer, [&]()
        {
            // Bind graphics pipeline with relevant shaders
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchain->getPipeline()->getHandle());

            // Bind vertex buffers
            std::vector<VkBuffer> vertexBuffers { vertexBuffer->getHandle() };
            std::vector<VkDeviceSize> vertexOffsets   { vertexBuffer->getOffset() };
            vkCmdBindVertexBuffers(commandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), vertexOffsets.data());

            // Bind index buffer
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT16);

            // Bind uniform
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchain->getPipeline()->getLayout(), 0, 1, &frame.getDescriptorSet().getHandle(), 0, nullptr);

            // Draw triangles
            vkCmdDrawIndexed(commandBuffer, indexBuffer->getNElements(), 1, 0, 0, 0);
        });
    });

    // Submit command buffer to main queue
    device->getMainQueue().drawSubmit(device, frame);
    
    // Present image
    device->getMainQueue().present(swapchain, frame, image);
}
