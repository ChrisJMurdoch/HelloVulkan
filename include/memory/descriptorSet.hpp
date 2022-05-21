
#pragma once

#include <vulkan/vulkan.h>

class Device;
class VoidBuffer;

class DescriptorSet
{
private:
    VkDescriptorSet const handle;

public:
    DescriptorSet(VkDescriptorSet const handle);
    
    VkDescriptorSet const &getHandle() const;

    void bindToBuffer(Device const *device, VoidBuffer const &buffer);
};
