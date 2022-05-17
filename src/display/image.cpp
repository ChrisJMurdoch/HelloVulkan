
#include "display/image.hpp"

Image::Image(VkImage const &image, VkImageView const &imageView, VkFramebuffer const &framebuffer, uint32_t const index)
    : image{image}, imageView{imageView}, framebuffer{framebuffer}, index{index}
{ }
