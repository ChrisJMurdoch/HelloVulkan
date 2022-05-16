
#pragma once

#include "display/window.hpp"
#include "display/instance.hpp"
#include "display/debugMessenger.hpp"
#include "display/surface.hpp"
#include "display/physicalDevice.hpp"
#include "display/device.hpp"
#include "display/swapchain.hpp"
#include "display/commandPool.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

enum BufferingStrategy
{
    SingleBuffering = 1,
    DoubleBuffering = 2,
    TripleBuffering = 3,
};

class Display
{
private:
    int const maxFramesInFlight;
    std::vector<const char *> activeValidationLayers;
    
    Window *window;
    Instance *instance;
    DebugMessenger *debugMessenger;
    Surface *surface;
    PhysicalDevice *physicalDevice; // TODO - possibly move or change to heap allocation
    uint32_t graphicsQueueFamilyIndex;
    Device *device;
    Swapchain *swapchain;
    CommandPool *commandPool;

    bool framebufferResized = false;

public:
    Display(int windowWidth, int windowHeight, BufferingStrategy bufferingStrategy=DoubleBuffering, bool enableValidationLayers=false);
    ~Display();

    void run();

private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void drawFrame();
};
