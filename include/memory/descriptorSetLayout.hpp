
#pragma once

#include <vulkan/vulkan.h>

class Device;

class DescriptorSetLayout
{
private:
    Device const *device;
    VkDescriptorSetLayout handle;

public:
    DescriptorSetLayout(Device const *device);
    ~DescriptorSetLayout();

    VkDescriptorSetLayout const &getHandle() const;
};
