
#include "swapchain/renderPass.hpp"

#include "configuration/device.hpp"
#include "swapchain/swapchain.hpp"
#include "swapchain/image.hpp"
#include "utility/check.hpp"

#include <exception>

RenderPass::RenderPass(Device const *device, VkFormat const &format) : device(device)
{
    VkAttachmentDescription colorAttachment
    {
        .format = format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    VkAttachmentReference colourAttachmentRef
    {
        colourAttachmentRef.attachment = 0,
        colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkSubpassDescription subpass
    {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colourAttachmentRef
    };
    VkSubpassDependency dependency
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };
    VkRenderPassCreateInfo renderPassInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };
    check::fail( vkCreateRenderPass(device->getHandle(), &renderPassInfo, nullptr, &handle), "vkCreateRenderPass failed." );
}

RenderPass::~RenderPass()
{
    vkDestroyRenderPass(device->getHandle(), handle, nullptr);
}

VkRenderPass const &RenderPass::getHandle() const
{
    return handle;
}

void RenderPass::run(Swapchain const *swapchain, Image const &image, VkCommandBuffer const &commandBuffer, std::function<void()> commands)
{
    // Start render pass
    VkClearValue clearColour = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = handle,
        .framebuffer = image.framebuffer,
        .renderArea{
            .offset = {0, 0},
            .extent = swapchain->getExtent()
        },
        .clearValueCount = 1,
        .pClearValues = &clearColour
    };
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Run commands
    commands();

    // End render pass
    vkCmdEndRenderPass(commandBuffer);
}
