
#include "frame/framePool.hpp"

#include "configuration/device.hpp"
#include "configuration/physicalDevice.hpp"
#include "command/commandPool.hpp"

FramePool::FramePool(Device const *device, CommandPool *commandPool, PhysicalDevice const *physicalDevice, int nFrames, DescriptorSetLayout const *descriptorSetLayout)
    : descriptorPool(device, nFrames, descriptorSetLayout)
{
    frames.reserve(nFrames);
    for (int i=0; i<nFrames; i++)
        frames.push_back(Frame(device, commandPool, physicalDevice, descriptorPool.getDescriptorSets()[i]));
}

Frame &FramePool::nextFrame()
{
    static int index = 0;
    Frame &frame = frames[index];
    index++; index%=frames.size();
    return frame;
}
