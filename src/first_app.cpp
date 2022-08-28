#include "first_app.hpp"
#include "GLFW/glfw3.h"
#include "bve_command_buffers.hpp"
#include "bve_device.hpp"
#include "bve_draw_frame.hpp"
#include "bve_model.hpp"
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include "bve_window.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>
#include <array>
namespace bve
{
    void runAppWithWindow(BveWindow* mainWindow)
    {
        //create vulkan device
        Device* device = deviceInit(mainWindow);
        //create swap chain
        SwapChain* swapchain = createBveSwapChain(device, getExtent(mainWindow));
        //load models into vector of models
        std::vector<Model*> models = loadModels(device);
        //create pipeline configuration with a hard coded default
        PipelineConfig* pipelineConfig = defaultPipelineConfigInfo(swapchain);

        //creating the pipe line itself using the coded default
        GraphicsPipeline* pipeline = 
            createGraphicsPipeline(device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", pipelineConfig);

        //Create command buffers. Should be a return value instead of a parameter!
        std::vector<VkCommandBuffer> commandBuffers;
        createCommandBuffers(pipeline, commandBuffers, swapchain, models);

        //--------------------------------------------
        //Drawing frames. This was copy pasted idk how it works rly yet.
        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
            drawFrame(swapchain, commandBuffers); 
        }

        vkDeviceWaitIdle(device->device_);
    }
}
