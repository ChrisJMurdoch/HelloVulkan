
#include "configuration/device.hpp"

#include "configuration/physicalDevice.hpp"
#include "configuration/queue.hpp"
#include "utility/check.hpp"

Device::Device(PhysicalDevice const *physicalDevice, std::vector<const char*> const &validationLayers, std::vector<const char*> const &extensions)
{
    // Create array of queues (just main queue for now)
    float const queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos
    {
        VkDeviceQueueCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = physicalDevice->getMainQueueFamilyIndex(),
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        }
    };

    // Create logical device
    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &deviceFeatures
    };
    check::fail( vkCreateDevice(physicalDevice->getHandle(), &createInfo, nullptr, &handle), "vkCreateDevice failed." );

    // Get generated queues
    vkGetDeviceQueue(handle, physicalDevice->getMainQueueFamilyIndex(), 0, &mainQueue);
}

Device::~Device()
{
    vkDestroyDevice(handle, nullptr);
}

VkDevice const &Device::getHandle() const
{
    return handle;
}

Queue Device::getMainQueue()
{
    return Queue(mainQueue);
}
