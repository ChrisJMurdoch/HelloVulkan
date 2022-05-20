
#pragma once

#include "command/commandBuffer.hpp"
#include "memory/typedBuffer.hpp"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class Device;
class CommandPool;
class PhysicalDevice;

struct UniformObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
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

public:
    Frame(Device const *device, CommandPool *commandPool, PhysicalDevice const *physicalDevice);
    Frame(Frame &&old);
    ~Frame();
    
    CommandBuffer const &getCommandBuffer() const;
    CommandBuffer &getCommandBuffer();
    VkSemaphore const &getImageAvailableSemaphore() const;
    VkSemaphore const &getRenderFinishedSemaphore() const;
    VkFence const &getInFlightFence() const;

    void waitForReady(Device const *device) const;

    void updateUniform(UniformObject const &uniform);
};
