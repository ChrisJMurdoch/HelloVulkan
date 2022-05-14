
#include "display/window.hpp"

Window::Window(int width, int height, GLFWframebuffersizefun callback)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    handle = glfwCreateWindow(width, height, "HelloVulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(handle, this);
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
