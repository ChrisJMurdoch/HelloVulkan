
#include "memory/descriptorSet.hpp"

#include "configuration/device.hpp"
#include "memory/voidBuffer.hpp"

DescriptorSet::DescriptorSet(VkDescriptorSet const handle) : handle(handle)
{ }

VkDescriptorSet const &DescriptorSet::getHandle() const
{
    return handle;
}

void DescriptorSet::bindToBuffer(Device const *device, VoidBuffer const &buffer)
{
    VkDescriptorBufferInfo bufferInfo
    {
        .buffer = buffer.getHandle(),
        .offset = 0,
        .range = buffer.getSize()
    };
    VkWriteDescriptorSet descriptorWrite
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = handle,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo
    };
    vkUpdateDescriptorSets(device->getHandle(), 1, &descriptorWrite, 0, nullptr);
}
