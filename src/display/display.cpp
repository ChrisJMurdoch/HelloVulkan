
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

// PARAMETERS

const int MAX_FRAMES_IN_FLIGHT = 2;
std::vector<const char*> const validationLayers{ "VK_LAYER_KHRONOS_validation" };
std::vector<const char*> const deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

// Inline error checking
inline void nullThrow (void *value,    char const *message = "Error: null value")         { if (value == nullptr)    throw std::exception(message); }
inline void failThrow (VkResult value, char const *message = "Error: vulkan call failed") { if (value != VK_SUCCESS) throw std::exception(message); }

// Exception debug callback, called by validation layers
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    if (messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT const debugMessengerCreateInfo =
{
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = debugCallback
};

// PUBLIC FUNCTIONS

Display::Display(int windowWidth, int windowHeight)
{
    initGlfw(windowWidth, windowHeight);
    initVulkan();
}

void Display::run() {
    mainLoop();
    cleanup();
}

// TIER 1

void Display::initGlfw(int windowWidth, int windowHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(windowWidth, windowHeight, "HelloVulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Display::initVulkan()
{
    instance = new Instance(debugMessengerCreateInfo, validationLayers);
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    this->qIndices = getQueueIndices();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createSyncObjects();
}

void Display::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        drawFrame();
    }
}

void Display::cleanup()
{
    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    delete commandPool;

    vkDestroyDevice(device, nullptr);

    auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance->getHandle(), "vkDestroyDebugUtilsMessengerEXT");
    if (destroyDebugUtilsMessenger != nullptr)
        destroyDebugUtilsMessenger(instance->getHandle(), debugMessenger, nullptr);

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    delete instance;

    glfwDestroyWindow(window);

    glfwTerminate();
}

// TIER 2

void Display::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    Display *display = reinterpret_cast<Display *>(glfwGetWindowUserPointer(window));
    display->framebufferResized = true;
}

void Display::cleanupSwapChain()
{
    for (VkFramebuffer const &framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);

    delete pipeline;
    delete renderPass;

    for (auto imageView : swapChainImageViews)
        vkDestroyImageView(device, imageView, nullptr);

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void Display::recreateSwapChain()
{
    // Get dimensions and block until window is visible
    int width=0, height=0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    // Cleanup old chain
    vkDeviceWaitIdle(device);
    cleanupSwapChain();

    // Create and activate new chain
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
}

void Display::setupDebugMessenger() {

    // Retrieve function pointer
    auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance->getHandle(), "vkCreateDebugUtilsMessengerEXT");
    nullThrow( createDebugUtilsMessenger, "Failed to set up debug messenger." );
    
    // Create messenger
    failThrow( createDebugUtilsMessenger(instance->getHandle(), &debugMessengerCreateInfo, nullptr, &debugMessenger), "Failed to set up debug messenger." );
}

void Display::createSurface()
{
    failThrow( glfwCreateWindowSurface(instance->getHandle(), window, nullptr, &surface), "Failed to create window surface." );
}

void Display::pickPhysicalDevice()
{
    // Get physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::exception("Failed to find GPUs with Vulkan support");
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, physicalDevices.data());

    // Select first suitable device
    for (VkPhysicalDevice const &physicalDevice : physicalDevices) {
        if (isDeviceSuitable(physicalDevice)) {
            this->physicalDevice = physicalDevice;
            break;
        }
    }

    // Ensure device is found
    nullThrow(physicalDevice, "Failed to find a suitable GPU.");
}

QueueFamilyIndices Display::getQueueIndices(VkPhysicalDevice  const &physicalDevice)
{
    // If no physical device is provided, use currently active
    VkPhysicalDevice const &physDevice = physicalDevice==nullptr ? this->physicalDevice : physicalDevice;
    nullThrow(physDevice, "No physical device active.");

    // Get queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies.data());

    // Get queue family indices
    QueueFamilyIndices indices;
    for (int i=0; i<queueFamilies.size(); i++)
    {
        VkQueueFamilyProperties const &queueFamily = queueFamilies[i];

        // Get graphics family
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        // Get present family
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;

        // Stop looking if complete
        if (indices.isComplete())
            break;
    }

    return indices;
}

void Display::createLogicalDevice()
{
    // Get unique queue families
    std::set<uint32_t> uniqueQueueFamilies{this->qIndices.graphicsFamily.value(), this->qIndices.presentFamily.value()};

    // Iterate each queue family
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        float const queuePriority = 1.0f;
        queueCreateInfos.push_back(VkDeviceQueueCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        });
    }

    // Create logical device
    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures
    };
    failThrow( vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Failed to create logical device" );

    // Get generated queues
    vkGetDeviceQueue(device, this->qIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, this->qIndices.presentFamily.value(), 0, &presentQueue);
}

void Display::createSwapChain()
{
    // Get swapchain support details of current physical device
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    // Choose optimal configurations
    VkSurfaceFormatKHR surfaceFormat = choose::swapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = choose::swapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = choose::swapExtent(swapChainSupport.capabilities, window);

    // Calculate optimal image count
    uint32_t minImg=swapChainSupport.capabilities.minImageCount, maxImg=swapChainSupport.capabilities.maxImageCount;
    uint32_t imageCount = minImg + 1;
    if (maxImg!=0 && maxImg<imageCount)
        imageCount = maxImg;

    // Create swapchain
    bool const multiQueues = this->qIndices.graphicsFamily != this->qIndices.presentFamily;
    uint32_t queueFamilyIndices[] = { this->qIndices.graphicsFamily.value(), this->qIndices.presentFamily.value() };
    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode =      multiQueues ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = multiQueues ? 2u : 0u,
        .pQueueFamilyIndices =   multiQueues ? queueFamilyIndices : nullptr,
        .preTransform = swapChainSupport.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE
    };
    failThrow( vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain), "Failed to create swapchain." );

    // Get generated images
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    // Save format data
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void Display::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (int i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapChainImageFormat,
            .components{
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        failThrow( vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]), "Failed to create image views." );
    }
}

void Display::createRenderPass()
{
    renderPass = new RenderPass(device, swapChainImageFormat);
}

void Display::createGraphicsPipeline()
{
    // Read shader files
    std::vector<char> vertShaderCode = io::readFile("shaders/bin/shader.vert.spv", std::ios::binary);
    std::vector<char> fragShaderCode = io::readFile("shaders/bin/shader.frag.spv", std::ios::binary);

    // Create shaders
    ShaderModule *vertShaderModule = new ShaderModule(device, vertShaderCode);
    ShaderModule *fragShaderModule = new ShaderModule(device, fragShaderCode);

    pipeline = new Pipeline(device, vertShaderModule, fragShaderModule, renderPass, swapChainExtent);

    // Clean up shaders
    delete fragShaderModule;
    delete vertShaderModule;
}

void Display::createFramebuffers()
{
    // One framebuffer for each image view
    swapChainFramebuffers.resize(swapChainImageViews.size());

    // For each image view
    for (int i = 0; i < swapChainImageViews.size(); i++) {

        // Create framebuffer
        VkImageView attachments[] = { swapChainImageViews[i] };
        VkFramebufferCreateInfo framebufferInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass->getHandle(),
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = swapChainExtent.width,
            .height = swapChainExtent.height,
            .layers = 1
        };
        failThrow( vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]), "Failed to create framebuffer." );
    }
}

void Display::createCommandPool()
{
    commandPool = new CommandPool(device, qIndices, MAX_FRAMES_IN_FLIGHT);
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
        failThrow( vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]), "Failed to create semaphore for a frame." );
        failThrow( vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]), "Failed to create semaphore for a frame." );
        failThrow( vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]),                   "Failed to create fence for a frame."     );
    }
}

void Display::drawFrame()
{
    // Wait for current frame to become available
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire image for current frame
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    switch (result)
    {
    case VK_ERROR_OUT_OF_DATE_KHR:
        recreateSwapChain();
        return;
    case VK_SUCCESS:
    case VK_SUBOPTIMAL_KHR:
        // OK
        break;
    default:
        throw std::exception("Failed to acquire swap chain image.");
    }
    
    // Reset frame resources
    vkResetFences(device, 1, &inFlightFences[currentFrame]);
    vkResetCommandBuffer(commandPool->getBuffers()[currentFrame], 0);

    recordCommandBuffer(commandPool->getBuffers()[currentFrame], imageIndex);

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
    failThrow( vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]), "Failed to submit draw command buffer." );

    // Present queue
    VkSwapchainKHR swapChains[] = {swapChain};
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex
    };
    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    // Potentially recreate swapchain
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = false;
        recreateSwapChain();
    }
    else
    {
        failThrow( result, "Failed to present swap chain image." );
    }

    // Change frame index to next
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// TIER 3

void Display::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // Begin command buffer
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    failThrow( vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer." );

    // Begin render pass
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass->getHandle(),
        .framebuffer = swapChainFramebuffers[imageIndex],
        .renderArea{
            .offset = {0, 0},
            .extent = swapChainExtent
        },
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    failThrow( vkEndCommandBuffer(commandBuffer), "Failed to record command buffer." );
}

SwapChainSupportDetails Display::querySwapChainSupport(VkPhysicalDevice const &physicalDevice)
{
    SwapChainSupportDetails details;

    // Get capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    // Get surface formats
    uint32_t nFormats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &nFormats, nullptr);
    details.formats.resize(nFormats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &nFormats, details.formats.data());

    // Get present modes
    uint32_t nPresentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &nPresentModes, nullptr);
    details.presentModes.resize(nPresentModes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &nPresentModes, details.presentModes.data());

    return details;
}

bool Display::isDeviceSuitable(VkPhysicalDevice physicalDevice)
{
    // Test queue family indices for specific device
    if (!getQueueIndices(physicalDevice).isComplete())
        return false;

    // Check extension support
    if ( !checkDeviceExtensionSupport(physicalDevice) )
        return false;
    
    // Check swapchain support
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
    if ( swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        return false;

    return true;
}

bool Display::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    // Get available extensions
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // Get required extensions
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    // Check each required extension is available
    for (VkExtensionProperties const &extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);
    return requiredExtensions.empty();
}
