#include "main_loader.h"
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
#include "entity.h"

// Logical and physical device that everything needs access to.
Device *device;
// Swap chain that everybody needs to be aware of
SwapChain *swapchain;

// This will load graphical resources from directories
// provided and build the descriptors and pipeline required. 
// BUT WAIT. There's a catch.
// It needs to be made in such a way that resources are reusable.
BBAPI P_BBEntity createEntity(char *model, char *texture, char *vertShader, char *fragShader)
{
    // replace this with init entity
    BBEntity *entity       = new BBEntity{};

    // This will need to actually load a model from a file
    entity->model          = loadModel(model);

    // This already loads a texture from a file!
    // yipee!
    entity->texture        = createTextureImage(texture, device);
                             createTextureImageView(entity->texture);
                             createTextureSampler(entity->texture);
    // All the uniform buffers associated with an entity.
    // Remember, each frame in the swap chain needs
    // a separate one!
    createUniformBuffers(entity->transformBuffer, device, sizeof(PerObjectMatrices));
    createUniformBuffers(entity->cameraBuffer, device, sizeof(ViewMatrices));

    // Vertex and index buffers from the loaded models
    createVertexBuffer(entity->vBuffer, device, entity->model);
    auto indexBuffers      = createIndexBuffer(device, entity->model);

    // create descriptor sets
    auto descriptorPool      = createDescriptorPool(device);
    auto descriptorSetLayout = createDescriptorSetLayout(device, DS_LAYOUT_BITCH_BASIC);
    auto descriptorSets      = createDescriptorSets(device, 
                                            descriptorSetLayout,
                                            descriptorPool, 
                                            uniformBuffers, 
                                            entity->texture);

    //create pipeline configuration with hard coded default shit
    auto pipelineConfig    = defaultPipelineConfigInfo(swapchain, uniformBuffers, descriptorSetLayout, descriptorSets);
    //creating the pipe line itself using the coded default
    auto pipeline          = createGraphicsPipeline(device, 
                                                  swapchain, 
                                                  vertShader, 
                                                  fragShader, 
                                                  pipelineConfig);
    return entity;
}

BBAPI void rotateEntity(BBEntity *entity, BBAxis axis)
{

}

// Once an entity is "created", it should be duplicated from then on.
BBAPI void spawnEntity(BBEntity *entity, double *worldCoords, int rotation)
{

}

BBAPI void initializeGFX(BBWindow *mainWindow)
{
    //create vulkan physical and logical device and store it all in device struct
    device                 = deviceInit(mainWindow);
    //create swap chain and store all the vulkan details in SwapChain struct
    swapchain              = createSwapChain(device, getExtent(mainWindow));
}

BBAPI void runAppWithWindow(BBWindow* mainWindow)
{
    initializeGFX(mainWindow);

    char model[]      = "whatever";
    char texture[]    = "../textures/CADE.png";
    char vertShader[] = "../shaders/simple_shader.vert.spv";
    char fragShader[] = "../shaders/simple_shader.frag.spv";
    BBEntity *entity0 = createEntity(model, texture, vertShader, fragShader);


    //Create command buffers. Should be a return value instead of a parameter!
    auto commandBuffers    = createCommandBuffers(entity0->pipeline);
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
    /*
    auto cleanupList = new CleanupList{device, 
                                       pipeline,
                                       swapchain,
                                       vertexBuffers,
                                       indexBuffers,
                                       uniformBuffers,
                                       models};
    cleanup(cleanupList);
    */
}
