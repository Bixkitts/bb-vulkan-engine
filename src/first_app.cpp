#include "first_app.hpp"
#include "GLFW/glfw3.h"
#include "bve_device.hpp"
#include "bve_pipeline.hpp"
#include "bve_window.hpp"

namespace bve
{
    void runAppWithWindow(BveWindow *mainWindow)
    {
        bve::BveDevice *device = bve::bveDeviceInit(mainWindow);
        bve::createGraphicsPipeline(device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", defaultPipelineConfigInfo(800, 600));
        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
        }
    }
}
