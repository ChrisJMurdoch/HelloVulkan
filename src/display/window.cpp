
#include "display/window.hpp"

Window::Window(int width, int height, GLFWframebuffersizefun callback, void *userPointer)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    handle = glfwCreateWindow(width, height, "HelloVulkan", nullptr, nullptr);
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        GLFWvidmode const *mode = glfwGetVideoMode(monitor);
        glfwSetWindowPos(handle, (mode->width - width) / 2.0, (mode->height - height) / 2.0);
    }
    glfwShowWindow(handle);
    glfwSetWindowUserPointer(handle, userPointer);
    glfwSetFramebufferSizeCallback(handle, callback);
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

GLFWwindow const *Window::getHandle() const
{
    return handle;
}

GLFWwindow *Window::getHandle()
{
    return handle;
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(handle);
}

void Window::getFrameBufferSize(int &width, int &height) const
{
    glfwGetFramebufferSize(handle, &width, &height);
}
