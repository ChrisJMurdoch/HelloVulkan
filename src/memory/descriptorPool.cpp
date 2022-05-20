
#include "memory/descriptorPool.hpp"

DescriptorPool::DescriptorPool()
{

}

DescriptorPool::~DescriptorPool()
{

}

VkDescriptorPool const &DescriptorPool::getHandle()
{
    return handle;
}
