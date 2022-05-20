
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Device;
class PhysicalDevice;
class Window;
class Surface;
class RenderPass;
class Pipeline;
class Image;
class Frame;

class Swapchain
{
private:
    VkSwapchainKHR handle;
    Device const *device;

    VkFormat format;
    VkExtent2D extent;

    RenderPass *renderPass;
    Pipeline *pipeline;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;

public:
    Swapchain(Device const *device, PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface);
    ~Swapchain();
    VkSwapchainKHR const &getHandle() const;
    VkExtent2D const &getExtent() const;
    RenderPass *getRenderPass();
    Pipeline const *getPipeline() const;
    Image const acquireNextImage(Frame const &frame, bool &framebufferResized, PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface);

private:
    void create(PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface);
    void destroy();
    void recreate(PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface);

    // Creation stages
    void createSwapchain(PhysicalDevice const *physicalDevice, Surface const *surface, Window const *window);
    static VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats);
    static VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes);
    static VkExtent2D chooseExtent(VkSurfaceCapabilitiesKHR const &capabilities, Window const *window);
    void createImageViews();
    void createFramebuffers();
};
