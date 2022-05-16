
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
private:
    GLFWwindow *handle;

public:
    Window(int width, int height, char const *title, GLFWframebuffersizefun callback, void *userPointer);
    ~Window();

    GLFWwindow *getHandle() const;
    bool shouldClose() const;
    void getFramebufferSize(int &width, int &height) const;
};
