
#include "swapchain/swapchain.hpp"

#include "configuration/device.hpp"
#include "configuration/physicalDevice.hpp"
#include "configuration/window.hpp"
#include "configuration/surface.hpp"
#include "configuration/renderPass.hpp"
#include "configuration/pipeline.hpp"
#include "swapchain/image.hpp"
#include "command/commandPool.hpp"
#include "command/commandBuffer.hpp"
#include "configuration/shaderModule.hpp"
#include "utility/check.hpp"
#include "utility/io.hpp"

#include <iostream>

Swapchain::Swapchain(Device const *device, PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface) : device(device)
{
    create(physicalDevice, window, surface);
}

Swapchain::~Swapchain()
{
    destroy();
}

void Swapchain::create(PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface)
{
    createSwapchain(physicalDevice, surface, window);
    createImageViews();
    renderPass = new RenderPass(device, format);
    pipeline = new Pipeline(
        device,
        ShaderModule(device, io::readFile("shaders/bin/shader.vert.spv", std::ios::binary)),
        ShaderModule(device, io::readFile("shaders/bin/shader.frag.spv", std::ios::binary)),
        renderPass, extent
    );
    createFramebuffers();
}

void Swapchain::destroy()
{
    for (VkFramebuffer const &framebuffer : framebuffers)
        vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
    delete pipeline;
    delete renderPass;
    for (auto imageView : imageViews)
        vkDestroyImageView(device->getHandle(), imageView, nullptr);
    vkDestroySwapchainKHR(device->getHandle(), handle, nullptr);
}

void Swapchain::recreate(PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface)
{
    // Get dimensions and block until window is visible
    int width=0, height=0;
    window->getFramebufferSize(width, height);
    while (width == 0 || height == 0)
    {
        glfwWaitEvents();
        window->getFramebufferSize(width, height);
    }

    // Wait until safe to recreate
    vkDeviceWaitIdle(device->getHandle());

    // Recreate
    destroy();
    create(physicalDevice, window, surface);
}

VkSwapchainKHR const &Swapchain::getHandle() const
{
    return handle;
}

VkExtent2D const &Swapchain::getExtent() const
{
    return extent;
}

RenderPass *Swapchain::getRenderPass()
{
    return renderPass;
}

Pipeline const *Swapchain::getPipeline() const
{
    return pipeline;
}

Image const Swapchain::acquireNextImage(CommandBuffer const &commandBuffer, bool &framebufferResized, PhysicalDevice const *physicalDevice, Window const *window, Surface const *surface)
{
    uint32_t imageIndex;
    while (
        framebufferResized ||
        vkAcquireNextImageKHR(device->getHandle(), handle, UINT64_MAX, commandBuffer.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS
    )
    {
        recreate(physicalDevice, window, surface);
        framebufferResized = false;
    }
    return Image(images[imageIndex], imageViews[imageIndex], framebuffers[imageIndex], imageIndex);
}

// Creation stages

void Swapchain::createSwapchain(PhysicalDevice const *physicalDevice, Surface const *surface, Window const *window)
{
    // Get swapchain support details of current physical device
    std::vector<VkSurfaceFormatKHR> const formats = surface->getFormats(physicalDevice->getHandle());
    std::vector<VkPresentModeKHR> const presentModes = surface->getPresentModes(physicalDevice->getHandle());
    VkSurfaceCapabilitiesKHR const capabilities = surface->getCapabilities(physicalDevice->getHandle());

    // Choose optimal configurations
    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(formats);
    format = surfaceFormat.format;
    VkPresentModeKHR presentMode = choosePresentMode(presentModes);
    extent = chooseExtent(capabilities, window);

    // Calculate optimal image count
    uint32_t minImg=capabilities.minImageCount, maxImg=capabilities.maxImageCount;
    uint32_t imageCount = minImg + 1;
    if (maxImg!=0 && maxImg<imageCount)
        imageCount = maxImg;

    // Create swapchain
    VkSwapchainCreateInfoKHR swapchainCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface->getHandle(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE
    };
    check::fail( vkCreateSwapchainKHR(device->getHandle(), &swapchainCreateInfo, nullptr, &handle), "vkCreateSwapchainKHR failed." );

    // Get generated images
    vkGetSwapchainImagesKHR(device->getHandle(), handle, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device->getHandle(), handle, &imageCount, images.data());
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats)
{
    for (VkSurfaceFormatKHR const &availableFormat : availableFormats)
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    
    std::cout << "Suboptimal surface format selected." << std::endl;
    return availableFormats[0];
}

VkPresentModeKHR Swapchain::choosePresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes)
{
    for (VkPresentModeKHR const &availablePresentMode : availablePresentModes)
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;

    std::cout << "Suboptimal present mode selected." << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseExtent(VkSurfaceCapabilitiesKHR const &capabilities, Window const *window)
{
    // Return default if extent is fixed
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    
    // Return best extent within capabilities' min and max
    int width, height;
    window->getFramebufferSize(width, height);
    return VkExtent2D
    {
        std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

void Swapchain::createImageViews()
{
    // For each image
    imageViews.resize(images.size());
    for (int i=0; i<images.size(); i++)
    {
        VkImageViewCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        check::fail( vkCreateImageView(device->getHandle(), &createInfo, nullptr, &imageViews[i]), "vkCreateImageView failed." );
    }
}

void Swapchain::createFramebuffers()
{
    // For each image view
    framebuffers.resize(imageViews.size());
    for (int i=0; i<imageViews.size(); i++)
    {
        std::vector<VkImageView> attachments = { imageViews[i] };
        VkFramebufferCreateInfo framebufferInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass->getHandle(),
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
            .width = extent.width,
            .height = extent.height,
            .layers = 1
        };
        check::fail( vkCreateFramebuffer(device->getHandle(), &framebufferInfo, nullptr, &framebuffers[i]), "vkCreateFramebuffer failed." );
    }
}
