#include "interface.hpp"

// Logical and physical device that everything needs access to.
static Device device = NULL;
// Swap chain that everybody needs to be aware of
static SwapChain swapchain;
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
BBAPI BBError createEntity(BBEntity entity, 
                           char *model, 
                           char *textureDir, 
                           char *vertShader, 
                           char *fragShader)
{
    BBDescriptorSetLayout dsLayout = DS_LAYOUT_BITCH_BASIC;
    // TODO: MALLOC without free
    entity = (BBEntity)calloc(1, sizeof(BBEntity));
    // This will need to actually load a model from a file
    const char *modelDir = "literally whatever";
    loadModel(entity->model, modelDir);
    // TODO:
    // I ought to be allocating resources from the memory pools up above!!
    // ---------------------------------------------------------------------------
    // This already loads a texture from a file!
    // yipee!
    createTextureImage     (entity->texture, 
                            textureDir, 
                            device);
    createTextureImageView (entity->texture);
    createTextureSampler   (entity->texture);
    // All the uniform buffers associated with an entity.
    // Remember, each frame in the swap chain needs
    // a separate one!
    createUniformBuffers   (entity->uBuffers, device, sizeof(PerObjectMatrices));
    createVertexBuffer     (&entity->vBuffer, device, entity->model);
    createIndexBuffer      (entity->iBuffer, device, entity->model);
    // TODO: instead of checking NULL maybe call this sort 
    // of stuff on init
    if (descriptorSetLayoutPool[dsLayout] == NULL){
        createDescriptorSetLayout(descriptorSetLayoutPool[dsLayout], device, dsLayout);
    }
    VkDescriptorSet *descriptorSets = NULL;
    // TODO: Descriptor set stuff
    createDescriptorSets   (descriptorSets,
                            device, 
                            descriptorSetLayoutPool[dsLayout],
                            descriptorPool, 
                            entity->uBuffers, 
                            entity->texture);
    VkPipelineLayout pipelineLayout = NULL;
    createPipelineLayout(&pipelineLayout,
                         device,
                         &descriptorSetLayoutPool[dsLayout]);
    //create pipeline configuration with hard coded default shit
    PipelineConfig pipelineConfig = NULL;
    createPipelineConfig   (pipelineConfig, 
                            swapchain, 
                            // TODO: uniform buffers here?
                            &entity->uBuffers, 
                            descriptorSetLayoutPool[dsLayout], 
                            descriptorSets,
                            pipelineLayout);
    //creating the pipe line itself using the coded default
    GraphicsPipeline pipeline = NULL;
    createGraphicsPipeline (pipeline,
                            device, 
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
    // TODO: return values
    device                 = deviceInit(mainWindow);
    swapchain              = createSwapChain(device, getExtent(mainWindow));
    createDescriptorPool   (descriptorPool, device);
}

BBAPI void runAppWithWindow(BBWindow* mainWindow)
{
    initializeGFX(mainWindow);

    char model[]      = "whatever";
    char texture[]    = "../textures/CADE.png";
    char vertShader[] = "../shaders/simple_shader.vert.spv";
    char fragShader[] = "../shaders/simple_shader.frag.spv";
    BBEntity entity0;
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
