
#pragma once

#include <vulkan/vulkan.h>

class Image
{
public:
    VkImage const &image;
    VkImageView const &imageView;
    VkFramebuffer const &framebuffer;
    uint32_t const index;

public:
    Image(VkImage const &image, VkImageView const &imageView, VkFramebuffer const &framebuffer, uint32_t const index);
};
