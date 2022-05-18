
#include "configuration/shaderModule.hpp"

#include "configuration/device.hpp"
#include "utility/check.hpp"

ShaderModule::ShaderModule(Device const *device, std::vector<char> const &code) : device(device)
{
    VkShaderModuleCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(code.data())
    };
    check::fail( vkCreateShaderModule(device->getHandle(), &createInfo, nullptr, &handle), "vkCreateShaderModule failed." );
}

ShaderModule::~ShaderModule()
{
    vkDestroyShaderModule(device->getHandle(), handle, nullptr);
}

VkShaderModule const &ShaderModule::getHandle() const
{
    return handle;
}
