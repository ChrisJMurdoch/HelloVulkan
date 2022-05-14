
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
private:
    GLFWwindow *handle;

public:
    Window(int width, int height, GLFWframebuffersizefun callback);
    ~Window();
    GLFWwindow const *getHandle() const;
    GLFWwindow *getHandle();
    bool shouldClose() const;
    void getFrameBufferSize(int &width, int &height) const;
};
