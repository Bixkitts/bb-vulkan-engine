#include "window.hpp"
#include "defines.hpp"
#include "GLFW/glfw3.h"

#include <stdexcept>


BBAPI int openWindow(BBWindow *window, const int width, const int height, const char *windowName)
{
    // TODO: check failure
    GLFWwindow *GLwindow = glfwCreateWindow(width, height, windowName, NULL, NULL);

    *window = (BBWindow)malloc(sizeof(BBWindow_T));

    glfwInit       ();
    glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

    (*window)->width   = width;
    (*window)->height  = height;
    (*window)->name    = windowName;
    (*window)->window  = GLwindow;

    return 0;
}

void closeWindow(BBWindow window)
{
    glfwDestroyWindow (window->window);
    glfwTerminate     ();
    free              (window);
}

void createWindowSurface(BBWindow window, VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window->window, nullptr, surface)){
        throw std::runtime_error("failed to create window surface");
    }
}

VkExtent2D getExtent(BBWindow window)
{
    VkExtent2D extent{static_cast<uint32_t>(window->width), static_cast<uint32_t>(window->height)};
    return extent;
}
