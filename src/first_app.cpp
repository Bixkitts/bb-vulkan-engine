#include "first_app.hpp"
#include "GLFW/glfw3.h"
#include "bve_window.hpp"

namespace bve
{
    void runAppWithWindow(BveWindow *mainWindow)
    {
        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
        }
    }
}
