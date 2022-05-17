
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Instance;
class Window;

class Surface
{
private:
    Instance const *instance;
    VkSurfaceKHR handle;

public:
    Surface(Instance const *instance, Window const *window);
    ~Surface();
    VkSurfaceKHR const &getHandle() const;
};
