
#pragma once

#include "frame/frame.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class CommandPool;

class FramePool
{
private:
    std::vector<Frame> frames;

public:
    FramePool(Device const *device, CommandPool *commandPool, int nFrames);
    Frame &nextFrame();
};
