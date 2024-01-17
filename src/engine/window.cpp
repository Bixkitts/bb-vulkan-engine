#include "window.hpp"
#include "defines.hpp"
#include "GLFW/glfw3.h"

#include <stdexcept>


BBAPI int openWindow(BBWindow *window, const int width, const int height, const char *windowName)
{
    glfwInit       ();
    glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

    // TODO: check failure
    GLFWwindow *GLwindow = glfwCreateWindow(width, height, windowName, NULL, NULL);

    *window = (BBWindow)malloc(sizeof(BBWindow_T));

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
        fprintf(stderr, "Failed to create window surface\n");
        exit(1);
    }
}

VkExtent2D getExtent(BBWindow window)
{
    VkExtent2D extent{static_cast<uint32_t>(window->width), static_cast<uint32_t>(window->height)};
    return extent;
}
