#include "bve_window.hpp"
#include "GLFW/glfw3.h"

#include <stdexcept>

namespace bve
{
    BveWindow* openWindow(int width, int height, std::string windowName)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        GLFWwindow *window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

        BveWindow *mainWindow = new BveWindow{width, height, window};

        return mainWindow;
    }

    void closeWindow(BveWindow *bveWindow)
    {
        glfwDestroyWindow(bveWindow->window);
        glfwTerminate();
        delete bveWindow;
    }
    void createWindowSurface(BveWindow *window, VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window->window, nullptr, surface))
            {
                throw std::runtime_error("failed to create window surface");
            }

    }

    VkExtent2D getExtent(BveWindow *window)
    {
        VkExtent2D extent{static_cast<uint32_t>(window->width), static_cast<uint32_t>(window->height)};
        return extent;
    }
}
