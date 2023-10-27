#ifndef BVE_WINDOW
#define BVE_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include "defines.hpp"

struct BBWindow
{
    const int width;
    const int height;
    GLFWwindow *window;
};

BBAPI BBWindow* openWindow(int w, int h, std::string name);
void closeWindow(BBWindow *window);
void createWindowSurface(BBWindow *window, VkInstance instance, VkSurfaceKHR *surface);
VkExtent2D getExtent(BBWindow *window);

#endif
