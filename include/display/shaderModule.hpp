
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class ShaderModule
{
private:
    VkDevice const &device;
    VkShaderModule handle;

public:
    ShaderModule(VkDevice const &device, std::vector<char> const &code);
    ~ShaderModule();
    VkShaderModule const &getHandle() const;
};
