#include "interface.hpp"
#include "GLFW/glfw3.h"
#include "buffers.hpp"
#include "command_buffers.hpp"
#include "device.hpp"
#include "draw_frame.hpp"
#include "descriptor_sets.hpp"
#include "model.hpp"
#include "pipeline.hpp" 
#include "swap_chain.hpp"
#include "window.hpp"
#include "cleanup.hpp"
#include "images.hpp"
#include "entity.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>
#include <array>
// Logical and physical device that everything needs access to.
Device *device;

SwapChain *swapchain;
// This should load a model from an external file
BBAPI BBEntity *createEntity(char *model, char *texture, char *vertShader, char *fragShader)
{
    BBEntity* entity       = new BBEntity{};
    entity->model          = loadModel(model);
    entity->texture        = createTextureImage(texture, device);
                             createTextureImageView(entity->texture);
                             createTextureSampler(entity->texture);
    auto uniformBuffers    = createUniformBuffers(device, sizeof(PerObjectMatrices));
    //Vertex and index buffers from the loaded models
    auto vertexBuffers     = createVertexBuffer(device, entity->model);
    auto indexBuffers      = createIndexBuffer(device, entity->model);

    // I need to have like a pool of pipelines and bind the right ones for the right objects
    // based on the shaders they use
}

    //create descriptor sets
    auto descriptorPool      = createDescriptorPool(device);
    auto descriptorSetLayout = createDescriptorSetLayout(device, BITCH_BASIC);
    auto descriptorSets      = createDescriptorSets(device, 
                                            descriptorSetLayout,
                                            descriptorPool, 
                                            uniformBuffers, 
                                            textureImageViews, 
                                            textureSamplers);

    //create pipeline configuration with a hard coded default
    auto pipelineConfig    = defaultPipelineConfigInfo(swapchain, uniformBuffers, descriptorSetLayout, descriptorSets);
    //creating the pipe line itself using the coded default
    auto pipeline          = createGraphicsPipeline(device, 
                                                  swapchain, 
                                                  "../shaders/simple_shader.vert.spv", 
                                                  "../shaders/simple_shader.frag.spv", 
                                                  pipelineConfig);

// 
BBAPI void *spawnEntity(BBEntity *entity, double *worldCoords, int rotation)
{
    auto uniformBuffers    = createUniformBuffers(device, sizeof(PerObjectMatrices));
    //Vertex and index buffers from the loaded models
    auto vertexBuffers     = createVertexBuffer(device, entity->model);
    auto indexBuffers      = createIndexBuffer(device, entity->model);

}
BBAPI void runAppWithWindow(BBWindow* mainWindow)
{
    //create vulkan physical and logical device and store it all in device struct
    device                 = deviceInit(mainWindow);
    //create swap chain and store all the vulkan details in SwapChain struct
    swapchain              = createSwapChain(device, getExtent(mainWindow));
    //load models into vector of models


//    auto object1           = spawnObject(model, texture, worldCoords);
    //texture related loading, buffering, views and sampling



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
        drawFrame(swapchain, pipeline, commandBuffers, uniformBuffers, vertexBuffers, indexBuffers, models); 
        // I need to turn "updateUniformBuffer()" into functions
        // for uniform buffer transformations on specific buffers.
        updateUniformBuffer(swapchain->currentFrame, swapchain, uniformBuffers);
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
