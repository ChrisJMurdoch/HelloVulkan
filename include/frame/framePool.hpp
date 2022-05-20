
#pragma once

#include "frame/frame.hpp"
#include "memory/descriptorPool.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class CommandPool;
class PhysicalDevice;

class FramePool
{
private:
    std::vector<Frame> frames;
    DescriptorPool descriptorPool;

public:
    FramePool(Device const *device, CommandPool *commandPool, PhysicalDevice const *physicalDevice, int nFrames);
    Frame &nextFrame();
};
