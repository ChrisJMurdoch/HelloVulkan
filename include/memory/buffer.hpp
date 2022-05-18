
#pragma once

#include "configuration/device.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class Vertex;
class PhysicalDevice;

class Buffer
{
private:
    VkBuffer handle;
    VkDeviceMemory memory;
    Device const *device;

protected:
    Buffer
    (
        Device const *device, PhysicalDevice const *physicalDevice, VkDeviceSize const &size,
        VkBufferUsageFlags const &usage, VkMemoryPropertyFlags const &properties
    );

public:
    ~Buffer();
    VkBuffer const &getHandle() const;
    uint32_t getOffset() const;

    template<class T>
    void memcpy(std::vector<T> sourceData)
    {
        // Map device memory
        void *deviceData;
        size_t size = sourceData.size() * sizeof(T);
        vkMapMemory(device->getHandle(), memory, 0, size, 0, &deviceData);

        // Copy data
        std::memcpy(deviceData, sourceData.data(), size);

        // Unmap
        vkUnmapMemory(device->getHandle(), memory);
    }
};
