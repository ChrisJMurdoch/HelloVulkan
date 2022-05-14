
#pragma once

#include "display/instance.hpp"
#include "display/window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Surface
{
private:
    Instance const *instance;
    VkSurfaceKHR handle;

public:
    Surface(Instance const *instance, Window *window);
    ~Surface();
    VkSurfaceKHR const &getHandle() const;
};
