
#include "display/surface.hpp"

#include "utility/check.hpp"

Surface::Surface(Instance const *instance, Window *window) : instance(instance)
{
    check::fail( glfwCreateWindowSurface(instance->getHandle(), window->getHandle(), nullptr, &handle), "glfwCreateWindowSurface failed." );
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(instance->getHandle(), handle, nullptr);
}

VkSurfaceKHR const &Surface::getHandle() const
{
    return handle;
}
