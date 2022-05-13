
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "display/renderPass.hpp"

#include <optional>
#include <vector>
#include <iostream>

// Store indices of each queue family
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// Store details of the swapchain
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Display
{
public:
    Display(int windowWidth, int windowHeight);
    void run();

private:
    GLFWwindow *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    QueueFamilyIndices qIndices;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    RenderPass *renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

private:

    // GLFW initialisation
    void initGlfw(int windowWidth, int windowHeight);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    // Vulkan initialisation
    void initVulkan();
        void createInstance();
            std::vector<char const *> getRequiredExtensions();
            bool checkValidationLayerSupport();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
            bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
                bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices getQueueIndices(VkPhysicalDevice  const &physicalDevice = nullptr);
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
            VkShaderModule createShaderModule(std::vector<char> const &code);
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void createSyncObjects();

    // Main render loop
    void mainLoop();
        void drawFrame();
            void recreateSwapChain();
            void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    // Free device memory
    void cleanup();

    // Referenced by multiple methods
    void cleanupSwapChain();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice const &physicalDevice);
};
