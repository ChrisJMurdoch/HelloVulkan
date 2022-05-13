
#include "display/shaderModule.hpp"

#include "utility/check.hpp"

ShaderModule::ShaderModule(VkDevice const &device, std::vector<char> const &code) : device(device)
{
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(code.data())
    };
    check::fail( vkCreateShaderModule(device, &createInfo, nullptr, &handle), "vkCreateShaderModule failed." );
}

ShaderModule::~ShaderModule()
{
    vkDestroyShaderModule(device, handle, nullptr);
}

VkShaderModule const &ShaderModule::getHandle() const
{
    return handle;
}
