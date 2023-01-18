#include "interface.hpp"
#include "GLFW/glfw3.h"
#include "bve_buffers.hpp"
#include "bve_command_buffers.hpp"
#include "bve_device.hpp"
#include "bve_draw_frame.hpp"
#include "bve_model.hpp"
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include "bve_window.hpp"
#include "bve_cleanup.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>
#include <array>
namespace bve
{
    BBAPI void runAppWithWindow(BveWindow* mainWindow)
    {
        //create vulkan physical and logical device and store it all in device struct
        auto device = deviceInit(mainWindow);
        //create swap chain and store all the vulkan details in SwapChain struct
        auto swapchain = createSwapChain(device, getExtent(mainWindow));
        //load models into vector of models
        auto models = loadModels(device);


        //Vertex buffer placeholder stuff
        auto vertexBuffers = createVertexBuffers(device, models);


        //create pipeline configuration with a hard coded default
        auto pipelineConfig = defaultPipelineConfigInfo(swapchain);
        //creating the pipe line itself using the coded default
        auto pipeline = 
            createGraphicsPipeline(device, swapchain, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", pipelineConfig);

        //Create command buffers. Should be a return value instead of a parameter!
        auto commandBuffers = createCommandBuffers(pipeline, swapchain, vertexBuffers);
        #ifdef DEBUG
        std::cout<<"\n -------This is a Debug build!-------\n";
        #endif
        //--------------------------------------------
        //Drawing frames. This was copy pasted idk how it works rly yet.
        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
            drawFrame(swapchain, commandBuffers); 
        }

        vkDeviceWaitIdle(device->logical);

        // Cleanup stuffs
        auto cleanupList = new CleanupList{device, 
                                           pipeline,
                                           swapchain,
                                           vertexBuffers};
        cleanup(cleanupList);
    }
}
