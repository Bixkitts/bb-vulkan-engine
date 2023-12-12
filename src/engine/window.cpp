#include "window.hpp"
#include "GLFW/glfw3.h"

#include <stdexcept>
typedef struct BBWindow_T {
    int width;
    int height;
    const char *name;
    GLFWwindow *window;
}BBWindow_T;

BBWindow openWindow(int width, int height, const char *windowName)
{
    GLFWwindow* window     = glfwCreateWindow(width, height, windowName, NULL, NULL);
    BBWindow    mainWindow = (BBWindow)malloc(sizeof(BBWindow_T));

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mainWindow->width   = width;
    mainWindow->height  = height;
    mainWindow->name    = windowName;
    mainWindow->window  = window;

    return mainWindow;
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
