#ifndef BB_VULKAN_LIB
#define BB_VULKAN_LIB

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

namespace bve
{
struct BveWindow
{
    const int width;
    const int height;
    GLFWwindow *window;
};

BveWindow* openWindow(int w, int h, std::string name);

void runAppWithWindow(BveWindow* mainWindow);


}

#endif
