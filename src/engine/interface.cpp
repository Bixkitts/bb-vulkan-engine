#include "interface.hpp"
#include "GLFW/glfw3.h"
#include "buffers.hpp"
#include "command_buffers.hpp"
#include "device.hpp"
#include "draw_frame.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window.hpp"
#include "cleanup.hpp"
#include "images.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>
#include <array>
namespace bve
{
    BBAPI void runAppWithWindow(BveWindow* mainWindow)
    {
        //create vulkan physical and logical device and store it all in device struct
        auto device            = deviceInit(mainWindow);
        //create swap chain and store all the vulkan details in SwapChain struct
        auto swapchain         = createSwapChain(device, getExtent(mainWindow));
        //load models into vector of models
        auto models            = loadModels(device);
        //Vertex and index buffers from the loaded models
        auto textureImage      = createTextureImage(device);
        auto textureImageView  = createTextureImageView(textureImage);
        auto textureSampler    = createTextureSampler(textureImage);

        auto vertexBuffers     = createVertexBuffers(device, models);
        auto indexBuffers      = createIndexBuffers(device, models);
        auto uniformBuffers    = createUniformBuffers(device, sizeof(Matrices));
        //create pipeline configuration with a hard coded default
        auto pipelineConfig    = defaultPipelineConfigInfo(swapchain, uniformBuffers);
        //creating the pipe line itself using the coded default
        auto pipeline          = createGraphicsPipeline(device, 
                                                      swapchain, 
                                                      "../shaders/simple_shader.vert.spv", 
                                                      "../shaders/simple_shader.frag.spv", 
                                                      pipelineConfig);

        //Create command buffers. Should be a return value instead of a parameter!
        auto commandBuffers    = createCommandBuffers(pipeline);
        #ifdef DEBUG
        std::cout<<"\n -------This is a Debug build!-------\n";
        #endif
        //--------------------------------------------
        //Drawing frames. This was copy pasted idk how it works rly yet.
        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
            drawFrame(swapchain, pipeline, commandBuffers, uniformBuffers, vertexBuffers, indexBuffers); 
        }

        vkDeviceWaitIdle(device->logical);

        // Cleanup stuffs
        auto cleanupList = new CleanupList{device, 
                                           pipeline,
                                           swapchain,
                                           vertexBuffers,
                                           indexBuffers,
                                           uniformBuffers,
                                           models};
        cleanup(cleanupList);
    }
}
