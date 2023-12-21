#include "command_buffers.hpp"
#include "interface.hpp"
#include "draw_frame.hpp"
#include "transform.hpp"
#include "window.hpp"

// Logical and physical device that everything needs access to.
static Device                device = NULL;
// Swap chain that everybody needs to be aware of
static SwapChain             swapchain;
// The big main descriptor pool
static VulkanDescriptorPool  descriptorPool;
// A pool of descriptor set layouts
static VkDescriptorSetLayout descriptorSetLayoutPool[DS_LAYOUT_COUNT] = { NULL };

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
BBAPI int createEntity(BBEntity *entity, 
                       const char *model, 
                       const char *textureDir, 
                       const char *vertShader, 
                       const char *fragShader)
{
    BBDescriptorSetLayout dsLayout       = DS_LAYOUT_BITCH_BASIC;
    VkDescriptorSetArray  descriptorSets = NULL;
    VkPipelineLayout      pipelineLayout = NULL;
    PipelineConfig        pipelineConfig = NULL;
    GraphicsPipeline      pipeline       = NULL;
    const char           *modelDir       = "literally whatever";

    // TODO: MALLOC without free
    *entity = (BBEntity)calloc(1, sizeof(BBEntity));
    // This will need to actually load a model froma file
    loadModel              ((*entity)->model, 
                            modelDir);
    createTextureImage     (&(*entity)->texture, 
                            textureDir, 
                            device);
    createTextureImageView ((*entity)->texture);
    createTextureSampler   ((*entity)->texture);
    // All the uniform buffers associated with an (*entity).
    // Remember, each frame in the swap chain needs
    // a separate one!
    createUniformBuffers   (&(*entity)->uBuffers, 
                            device, 
                            sizeof(PerObjectMatrices_T));
    createVertexBuffer     (&(*entity)->vBuffer, 
                            device, 
                            (*entity)->model);
    createIndexBuffer      (&(*entity)->iBuffer, 
                            device, 
                            (*entity)->model);
    // TODO: instead of checking NULL maybe call this sort 
    // of stuff on init
    if (descriptorSetLayoutPool[dsLayout] == NULL){
        createDescriptorSetLayout(&descriptorSetLayoutPool[dsLayout], 
                                  device, 
                                  dsLayout);
    }
    createDescriptorSets   (&descriptorSets,
                            device, 
                            descriptorSetLayoutPool[dsLayout],
                            descriptorPool, 
                            (*entity)->uBuffers, 
                            (*entity)->texture);
    createPipelineLayout   (&pipelineLayout,
                            device,
                            &descriptorSetLayoutPool[dsLayout]);
    //create pipeline configuration with hard coded default shit
    createPipelineConfig   (&pipelineConfig, 
                            swapchain, 
                            // TODO: uniform buffers here?
                            (*entity)->uBuffers, 
                            descriptorSetLayoutPool[dsLayout], 
                            descriptorSets,
                            pipelineLayout);
    //creating the pipe line itself using the coded default
    createGraphicsPipeline (pipeline,
                            device, 
                            swapchain, 
                            vertShader, 
                            fragShader, 
                            pipelineConfig);
    return 0;
}

BBAPI void rotateEntity(BBEntity *entity, BBAxis axis)
{

}

// Once an entity is "created", it should be duplicated from then on.
BBAPI void spawnEntity(BBEntity *entity, double *worldCoords, int rotation)
{

}

BBAPI int initializeGFX(const BBWindow mainWindow)
{
    deviceInit           (&device, mainWindow);
    createSwapChain      (&swapchain, device, getExtent(mainWindow));
    createDescriptorPool (descriptorPool, device);
    return 0;
}

BBAPI void runAppWithWindow(BBWindow mainWindow)
{
    // TODO: do I make this global or smthn
    VkCommandBufferArray primaryCommandBuffers = NULL;
    BBEntity             entity0               = NULL;
    const char           model[]               = "whatever";
    const char           texture[]             = "../textures/CADE.png";
    const char           vertShader[]          = "../shaders/simple_shader.vert.spv";
    const char           fragShader[]          = "../shaders/simple_shader.frag.spv";

    initializeGFX               (mainWindow);
    createEntity                (&entity0, model, texture, vertShader, fragShader);
    createPrimaryCommandBuffers (&primaryCommandBuffers, device);
    #ifdef DEBUG
    std::cout<<"\n -------This is a Debug build!-------\n";
    #endif
    while (!glfwWindowShouldClose(mainWindow->window))
    {
        glfwPollEvents      ();
        drawFrame           (swapchain, primaryCommandBuffers, &entity0, 1); 
        updateUniformBuffer (swapchain, entity0->uBuffers);
    }

    vkDeviceWaitIdle(device->logical);

    // TODO: Cleanup stuffs
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
