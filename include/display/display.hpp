
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "display/renderPass.hpp"
#include "display/pipeline.hpp"
#include "display/commandPool.hpp"
#include "display/instance.hpp"
#include "display/debugMessenger.hpp"
#include "display/physicalDevice.hpp"
#include "display/device.hpp"
#include "display/window.hpp"
#include "display/surface.hpp"
#include "display/swapchain.hpp"

#include <optional>
#include <vector>

class Display
{
private:
    Window *window;
    Instance *instance;
    DebugMessenger *debugMessenger;
    PhysicalDevice *physicalDevice; // TODO - possibly move or change to heap allocation
    Device *device;
    uint32_t graphicsQueueFamilyIndex;
    CommandPool *commandPool;
    Surface *surface;
    Swapchain *swapChain;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

public:
    Display(int windowWidth, int windowHeight);
    ~Display();

    void run();

private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void drawFrame();
};
