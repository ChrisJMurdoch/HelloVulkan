
#pragma once

#include "memory/descriptorSet.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class DescriptorSetLayout;

class DescriptorPool
{
private:
    Device const *device;
    VkDescriptorPool handle;
    std::vector<DescriptorSet> descriptorSets;

public:
    DescriptorPool(Device const *device, int nDescriptorSets, DescriptorSetLayout const *descriptorSetLayout);
    ~DescriptorPool();

    VkDescriptorPool const &getHandle();
    std::vector<DescriptorSet> &getDescriptorSets();
};
