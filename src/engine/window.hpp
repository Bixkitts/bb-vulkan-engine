#ifndef BVE_WINDOW
#define BVE_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct BBWindow_T {
    int width;
    int height;
    const char *name;
    GLFWwindow *window;
}BBWindow_T, *BBWindow;

void           closeWindow         (BBWindow window);
void           createWindowSurface (BBWindow window, 
                                    VkInstance instance, 
                                    VkSurfaceKHR *surface);
VkExtent2D     getExtent           (BBWindow window);

#endif
