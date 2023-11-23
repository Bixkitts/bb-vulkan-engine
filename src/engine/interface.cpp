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
static Device *device;
// Swap chain that everybody needs to be aware of
static SwapChain *swapchain;
// The big main descriptor pool
static VulkanDescriptorPool *descriptorPool;
// A pool of descriptor set layouts
static VkDescriptorSetLayout descriptorSetLayoutPool[DS_LAYOUT_AMOUNT_OF_LAYOUTS] = { NULL };

// TODO: these do nothing yet!!
// Have allocated memory pools to create buffers in
// Maybe make these like their own struct with
// more metadata about the allocation
static VkDeviceMemory *memoryPool;
static VkDeviceMemory *anotherMemoryPool;
static VkDeviceMemory *yetAnotherMemoryPool;
// This will load graphical resources from directories
// provided and build the descriptors and pipeline required. 
// BUT WAIT. There's a catch.
// It needs to be made in such a way that resources are reusable.
BBAPI BBError createEntity(P_BBEntity entity, char *model, char *textureDir, char *vertShader, char *fragShader)
{
    // TODO: MALLOC without free
    // Maybe replace this with a constructor
    entity = (BBEntity*)calloc(1, sizeof(BBEntity));

    // This will need to actually load a model from a file
    const char *modelDir = "literally whatever";
    loadModel(entity->model, modelDir);

    // TODO:
    // I ought to be allocating resources from the memory pools up above!!
    // ---------------------------------------------------------------------------
    // This already loads a texture from a file!
    // yipee!
    createTextureImage(entity->texture, textureDir, device);
    createTextureImageView(entity->texture);
    createTextureSampler(entity->texture);
    // All the uniform buffers associated with an entity.
    // Remember, each frame in the swap chain needs
    // a separate one!
    createUniformBuffers(entity->transformBuffer, device, sizeof(PerObjectMatrices));
    createUniformBuffers(entity->cameraBuffer, device, sizeof(ViewMatrices));

    // Vertex and index buffers from the loaded models
    createVertexBuffer(entity->vBuffer, device, entity->model);
    createIndexBuffer(entity->iBuffer, device, entity->model);

    // create descriptor sets
    BBDescriptorSetLayout dsLayout = DS_LAYOUT_BITCH_BASIC;
    createDescriptorPool(descriptorPool, device);
    if (descriptorSetLayoutPool[dsLayout] == NULL)
        createDescriptorSetLayout(descriptorSetLayoutPool[dsLayout], device, dsLayout);
    createDescriptorSets(
                         device, 
                         descriptorSetLayoutPool[dsLayout],
                         descriptorPool, 
                         entity->transformBuffer, 
                         entity->texture);

    //create pipeline configuration with hard coded default shit
    auto pipelineConfig    = defaultPipelineConfigInfo(swapchain, uniformBuffers, descriptorSetLayout, descriptorSets);
    //creating the pipe line itself using the coded default
    auto pipeline          = createGraphicsPipeline(device, 
                                                  swapchain, 
                                                  vertShader, 
                                                  fragShader, 
                                                  pipelineConfig);
    return BB_ERROR_OK;
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
    P_BBEntity entity0;
    createEntity(entity0, model, texture, vertShader, fragShader);


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
