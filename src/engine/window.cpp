#include "window.hpp"
#include "GLFW/glfw3.h"

#include <stdexcept>

BBAPI BBWindow* openWindow(int width, int height, std::string windowName)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

    BBWindow* mainWindow = new BBWindow{width, height, window};

    return mainWindow;
}

void closeWindow(BBWindow* bveWindow)
{
    glfwDestroyWindow(bveWindow->window);
    glfwTerminate();
    //remember other deallocation stuffs here maybe
    //
    delete bveWindow;
}

void createWindowSurface(BBWindow* window, VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window->window, nullptr, surface))
        {
            throw std::runtime_error("failed to create window surface");
        }

}

VkExtent2D getExtent(BBWindow* window)
{
    VkExtent2D extent{static_cast<uint32_t>(window->width), static_cast<uint32_t>(window->height)};
    return extent;
}
