
#pragma once

#include "command/commandBuffer.hpp"
#include "memory/typedBuffer.hpp"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class Device;
class CommandPool;
class PhysicalDevice;
class DescriptorSet;

struct UniformObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

/** Stores all per-frame-in-flight data necessary */
class Frame
{
private:
    Device const *device;
    CommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    TypedBuffer<UniformObject> uniformObjectBuffer;
    DescriptorSet const &descriptorSet;

public:
    Frame(Device const *device, CommandPool *commandPool, PhysicalDevice const *physicalDevice, DescriptorSet &descriptorSet);
    Frame(Frame &&old);
    ~Frame();
    
    CommandBuffer const &getCommandBuffer() const;
    CommandBuffer &getCommandBuffer();
    VkSemaphore const &getImageAvailableSemaphore() const;
    VkSemaphore const &getRenderFinishedSemaphore() const;
    VkFence const &getInFlightFence() const;
    DescriptorSet const &getDescriptorSet() const;

    void waitForReady(Device const *device) const;

    void updateUniform(UniformObject const &uniform);
};
