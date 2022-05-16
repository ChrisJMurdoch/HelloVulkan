
#include "display/swapchain.hpp"

#include "display/renderPass.hpp"
#include "utility/check.hpp"
#include "utility/io.hpp"

#include <iostream>

Swapchain::Swapchain(Device const *device, PhysicalDevice const *physicalDevice, Window *window, Surface const *surface) : device(device)
{
    create(physicalDevice, window, surface);
}

void Swapchain::create(PhysicalDevice const *physicalDevice, Window *window, Surface const *surface)
{
    createSwapChain(physicalDevice, surface, window);
    createImageViews();
    renderPass = new RenderPass(device->getHandle(), swapChainImageFormat);
    pipeline = new Pipeline(
        device->getHandle(),
        ShaderModule(device->getHandle(), io::readFile("shaders/bin/shader.vert.spv", std::ios::binary)),
        ShaderModule(device->getHandle(), io::readFile("shaders/bin/shader.frag.spv", std::ios::binary)),
        renderPass, swapChainExtent
    );
    createFramebuffers();
}

Swapchain::~Swapchain()
{
    cleanupSwapChain();
}

void Swapchain::cleanupSwapChain()
{
    for (VkFramebuffer const &framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
    delete pipeline;
    delete renderPass;
    for (auto imageView : swapChainImageViews)
        vkDestroyImageView(device->getHandle(), imageView, nullptr);
    vkDestroySwapchainKHR(device->getHandle(), handle, nullptr);
}

void Swapchain::recreateSwapChain(PhysicalDevice const *physicalDevice, Window *window, Surface const *surface)
{
    // Get dimensions and block until window is visible
    int width=0, height=0;
    window->getFrameBufferSize(width, height);
    while (width == 0 || height == 0)
    {
        window->getFrameBufferSize(width, height);
        glfwWaitEvents();
    }

    // Cleanup old chain
    vkDeviceWaitIdle(device->getHandle());
    cleanupSwapChain();

    // Create and activate new chain
    create(physicalDevice, window, surface);
}

VkSwapchainKHR const &Swapchain::getHandle() const
{
    return handle;
}

std::vector<VkFramebuffer> const &Swapchain::getFramebuffers() const
{
    return swapChainFramebuffers;
}

VkExtent2D const &Swapchain::getExtent() const
{
    return swapChainExtent;
}

RenderPass const *Swapchain::getRenderPass() const
{
    return renderPass;
}

Pipeline const *Swapchain::getPipeline() const
{
    return pipeline;
}

void Swapchain::createSwapChain(PhysicalDevice const *physicalDevice, Surface const *surface, Window *window)
{
    // Get swapchain support details of current physical device
    SwapChainSupportDetails swapChainSupport = PhysicalDevice::querySwapChainSupport(physicalDevice->getHandle(), surface->getHandle());

    // Choose optimal configurations
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window->getHandle());

    // Calculate optimal image count
    uint32_t minImg=swapChainSupport.capabilities.minImageCount, maxImg=swapChainSupport.capabilities.maxImageCount;
    uint32_t imageCount = minImg + 1;
    if (maxImg!=0 && maxImg<imageCount)
        imageCount = maxImg;
    std::cout << "Using " << imageCount << " (limits: " << minImg << "-" << maxImg << ") images in swapchain." << std::endl;

    // Create swapchain
    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface->getHandle(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = swapChainSupport.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE
    };
    check::fail( vkCreateSwapchainKHR(device->getHandle(), &createInfo, nullptr, &handle), "vkCreateSwapchainKHR failed." );

    // Get generated images
    vkGetSwapchainImagesKHR(device->getHandle(), handle, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device->getHandle(), handle, &imageCount, swapChainImages.data());

    // Save format data
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats)
{
    for (VkSurfaceFormatKHR const &availableFormat : availableFormats)
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;

    return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes)
{
    for (VkPresentModeKHR const &availablePresentMode : availablePresentModes)
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(VkSurfaceCapabilitiesKHR const &capabilities, GLFWwindow *window)
{
    // Return default if extent is fixed
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    
    // Return best extent within capabilities' min and max
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return VkExtent2D{
        std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

void Swapchain::createImageViews()
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
        check::fail( vkCreateImageView(device->getHandle(), &createInfo, nullptr, &swapChainImageViews[i]), "FvkCreateImageView failed." );
    }
}

void Swapchain::createFramebuffers()
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
        check::fail( vkCreateFramebuffer(device->getHandle(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]), "Failed to create framebuffer." );
    }
}
