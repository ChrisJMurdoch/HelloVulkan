
#include "display/window.hpp"

Window::Window(int width, int height, char const *title, GLFWframebuffersizefun callback, void *userPointer)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    GLFWvidmode const *mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(handle, (mode->width - width) / 2.0, (mode->height - height) / 2.0);
    glfwShowWindow(handle);

    glfwSetWindowUserPointer(handle, userPointer);
    glfwSetFramebufferSizeCallback(handle, callback);
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

GLFWwindow *Window::getHandle() const
{
    return handle;
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(handle);
}

void Window::getFramebufferSize(int &width, int &height) const
{
    glfwGetFramebufferSize(handle, &width, &height);
}
