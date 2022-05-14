
#pragma once

#include <vulkan/vulkan.h>

#include "display/shaderModule.hpp"
#include "display/renderPass.hpp"

class Pipeline
{
private:
    VkDevice const &device;
    VkPipeline handle;
    VkPipelineLayout pipelineLayout;

public:
    Pipeline(VkDevice const &device, ShaderModule const &vertShaderModule, ShaderModule const &fragShaderModule, RenderPass const *renderPass, VkExtent2D const &viewportExtent);
    ~Pipeline();
    VkPipeline const &getHandle() const;
};
