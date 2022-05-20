
#include "memory/descriptorSetLayout.hpp"

#include "configuration/device.hpp"
#include "utility/check.hpp"

DescriptorSetLayout::DescriptorSetLayout(Device const *device) : device(device)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding
    {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };
    VkDescriptorSetLayoutCreateInfo layoutInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &uboLayoutBinding
    };
    check::fail( vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &handle), "vkCreateDescriptorSetLayout" );
};

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(device->getHandle(), handle, nullptr);
};

VkDescriptorSetLayout const &DescriptorSetLayout::getHandle() const
{
    return handle;
}
