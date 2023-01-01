#include "first_app.hpp"
#include "GLFW/glfw3.h"
#include "bve_buffer_vertex.hpp"
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
        //create vulkan physical and logical device and store it all in device struct
        auto device = deviceInit(mainWindow);
        //create swap chain and store all the vulkan details in SwapChain struct
        auto swapchain = createSwapChain(device, getExtent(mainWindow));
        //load models into vector of models
        auto models = loadModels(device);
        //Vertex buffer placeholder stuff
        auto vertexBuffer = createVertexBuffer(device, models[0]->size());
        std::vector<VertexBuffer*> vertexBuffers = {vertexBuffer};
        //copy the stuff to the device, look into how this works
        copyToDevice(vertexBuffer, models[0]);
        //create pipeline configuration with a hard coded default
        std::cout << "done copying to device.... \n";
        auto pipelineConfig = defaultPipelineConfigInfo(swapchain);
        std::cout << "default pipeline config loaded... \n";
        //creating the pipe line itself using the coded default
        auto pipeline = 
            createGraphicsPipeline(device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", pipelineConfig);

        //Create command buffers. Should be a return value instead of a parameter!
        auto commandBuffers = createCommandBuffers(pipeline, swapchain, vertexBuffers);

        //--------------------------------------------
        //Drawing frames. This was copy pasted idk how it works rly yet.
        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
            drawFrame(swapchain, commandBuffers); 
        }

        vkDeviceWaitIdle(device->logical);
    }
}
