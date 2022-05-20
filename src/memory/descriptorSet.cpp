
#include "memory/descriptorSet.hpp"

DescriptorSet::DescriptorSet(VkDescriptorSet const &handle) : handle(handle)
{ }

VkDescriptorSet const &DescriptorSet::getHandle()
{
    return handle;
}
