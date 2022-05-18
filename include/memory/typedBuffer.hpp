
#pragma once

#include "memory/voidBuffer.hpp"
#include "utility/util.hpp"

#include <vulkan/vulkan.h>

template<class T>
class TypedBuffer : public VoidBuffer
{
public:
    TypedBuffer
    (
        Device const *device, PhysicalDevice const *physicalDevice, VkDeviceSize const &size,
        VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties
    )
        : VoidBuffer (device, physicalDevice, size, usage, properties)
    { }

    int getNElements() const
    {
        return size / sizeof(T);
    }

    void memcpy(std::vector<T> const &sourceData)
    {
        VoidBuffer::memcpy(util::vecsizeof(sourceData), sourceData.data());
    }
};
