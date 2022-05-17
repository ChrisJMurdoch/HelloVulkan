
#pragma once

#include "display/device.hpp"
#include "display/physicalDevice.hpp"
#include "display/window.hpp"
#include "display/surface.hpp"
#include "display/pipeline.hpp"

#include <vulkan/vulkan.h>

class RenderPass;
class CommandPool;
class CommandBuffer;

class Image
{
public:
    VkImage const &image;
    VkImageView const &imageView;
    VkFramebuffer const &framebuffer;
    uint32_t const index;
    Image(VkImage const &image, VkImageView const &imageView, VkFramebuffer const &framebuffer, uint32_t const index);
};

class Swapchain
{
private:
    Device const *device;
    VkSwapchainKHR handle;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    RenderPass *renderPass;
    Pipeline *pipeline;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

public:
    Swapchain(Device const *device, PhysicalDevice const *physicalDevice, Window *window, Surface const *surface);
    void create(PhysicalDevice const *physicalDevice, Window *window, Surface const *surface);
    ~Swapchain();
    void cleanupSwapChain();
    void recreateSwapChain(PhysicalDevice const *physicalDevice, Window *window, Surface const *surface);
    VkSwapchainKHR const &getHandle() const;
    Image const getImage(int index) const;
    VkExtent2D const &getExtent() const;
    RenderPass const *getRenderPass() const;
    Pipeline const *getPipeline() const;

    void createSwapChain(PhysicalDevice const *physicalDevice, Surface const *surface, Window *window);
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes);
    static VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR const &capabilities, GLFWwindow *window);
    void createImageViews();
    void createFramebuffers();

    Image const acquireNextImage(CommandBuffer const &commandBuffer, bool &framebufferResized, PhysicalDevice const *physicalDevice, Window *window, Surface const *surface);
};
