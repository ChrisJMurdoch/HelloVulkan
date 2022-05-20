
#pragma once

#include <vulkan/vulkan.h>

class DescriptorSet
{
private:
    VkDescriptorSet const &handle;

public:
    DescriptorSet(VkDescriptorSet const &handle);
    
    VkDescriptorSet const &getHandle();
};
