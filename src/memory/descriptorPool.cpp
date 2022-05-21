
#include "memory/descriptorPool.hpp"

#include "configuration/device.hpp"
#include "memory/descriptorSetLayout.hpp"
#include "utility/check.hpp"

DescriptorPool::DescriptorPool(Device const *device, int nDescriptorSets, DescriptorSetLayout const *descriptorSetLayout) : device(device)
{
    // Create pool
    VkDescriptorPoolSize poolSize
    {
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        poolSize.descriptorCount = static_cast<uint32_t>(nDescriptorSets)
    };
    VkDescriptorPoolCreateInfo poolInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = static_cast<uint32_t>(nDescriptorSets),
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };
    check::fail(vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &handle), "vkCreateDescriptorPool failed." );

    // Create sets
    std::vector<VkDescriptorSetLayout> layouts(nDescriptorSets, descriptorSetLayout->getHandle());
    VkDescriptorSetAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = handle,
        .descriptorSetCount = static_cast<uint32_t>(nDescriptorSets),
        .pSetLayouts = layouts.data()
    };
    std::vector<VkDescriptorSet> descriptorSetHandles(nDescriptorSets);
    check::fail( vkAllocateDescriptorSets(device->getHandle(), &allocInfo, descriptorSetHandles.data()), "vkAllocateDescriptorSets failed." );

    // Create set objects
    for (VkDescriptorSet descriptorSetHandle : descriptorSetHandles)
        descriptorSets.push_back(DescriptorSet(descriptorSetHandle));
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(device->getHandle(), handle, nullptr);
}

VkDescriptorPool const &DescriptorPool::getHandle()
{
    return handle;
}

std::vector<DescriptorSet> &DescriptorPool::getDescriptorSets()
{
    return descriptorSets;
}
