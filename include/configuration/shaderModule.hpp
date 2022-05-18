
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Device;

class ShaderModule
{
private:
    VkShaderModule handle;
    Device const *device;

public:
    ShaderModule(Device const *device, std::vector<char> const &code);
    ~ShaderModule();
    VkShaderModule const &getHandle() const;
};
