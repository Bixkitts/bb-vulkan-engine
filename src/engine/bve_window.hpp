#ifndef BVE_WINDOW
#define BVE_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace bve
{
    struct BveWindow
    {
        const int width;
        const int height;
        GLFWwindow *window;
    };

    BveWindow* openWindow(int w, int h, std::string name);
    void closeWindow(GLFWwindow *window);
    void createWindowSurface(BveWindow *window, VkInstance instance, VkSurfaceKHR *surface);
    VkExtent2D getExtent(BveWindow *window);
}

#endif
