
#pragma once

#include "memory/descriptorSet.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class DescriptorPool
{
private:
    VkDescriptorPool handle;
    std::vector<DescriptorSet> descriptorSets;

public:
    DescriptorPool();
    ~DescriptorPool();

    VkDescriptorPool const &getHandle();
};
