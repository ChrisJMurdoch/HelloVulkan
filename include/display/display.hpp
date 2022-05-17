
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window;
class Instance;
class DebugMessenger;
class Surface;
class PhysicalDevice;
class Device;
class Swapchain;
class CommandPool;
class VertexBuffer;

enum BufferingStrategy
{
    SingleBuffering = 1,
    DoubleBuffering = 2,
    TripleBuffering = 3,
};

class Display
{
private:
    Window *window;
    Instance *instance;
    DebugMessenger *debugMessenger;
    Surface *surface;
    PhysicalDevice *physicalDevice;
    Device *device;
    Swapchain *swapchain;
    CommandPool *commandPool;
    VertexBuffer *vertexBuffer;

    bool framebufferResized = false;

public:
    Display(int windowWidth, int windowHeight, char const *title, BufferingStrategy bufferingStrategy=DoubleBuffering, bool enableValidationLayers=false);
    ~Display();

    void tick();
    bool shouldClose() const;

private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void drawFrame();
};
