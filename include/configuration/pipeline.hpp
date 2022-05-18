
#pragma once

#include <vulkan/vulkan.h>

class Device;
class ShaderModule;
class RenderPass;

class Pipeline
{
private:
    Device const *device;
    VkPipeline handle;
    VkPipelineLayout pipelineLayout;

public:
    Pipeline(Device const *device, ShaderModule const &vertShaderModule, ShaderModule const &fragShaderModule, RenderPass const *renderPass, VkExtent2D const &viewportExtent);
    ~Pipeline();
    VkPipeline const &getHandle() const;
};
