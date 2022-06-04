#include "bve_window.hpp"
#include "GLFW/glfw3.h"

namespace bve
{
    BveWindow* openWindow(int width, int height, std::string windowName)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        GLFWwindow *window = nullptr;
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

        BveWindow *mainWindow = new BveWindow{width, height, window};

        return mainWindow;
    }

    void closeWindow(BveWindow *bveWindow)
    {
        glfwDestroyWindow(bveWindow->window);
        glfwTerminate();
        delete bveWindow;
    }
}
