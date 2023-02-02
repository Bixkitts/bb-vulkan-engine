#include "buffers.hpp"
#include "pipeline.hpp"
#include "window.hpp"
#include "cleanup.hpp"
#include "swap_chain.hpp"
#include "device.hpp"
namespace bve
{
// more things will... eventually be listed here!
void cleanup(CleanupList *list)
{
    closeWindow(list->device->window);
    destroySwapchain(list->swapchain);
    destroyPipeline(list->pipeline);
    
    for(int i = 0; i < list->vertexBuffers.size();i++)
    {
        destroyBuffer(list->vertexBuffers[i]);
    }
    
    for(int i = 0; i < list->indexBuffers.size();i++)
    {
        destroyBuffer(list->indexBuffers[i]);
    }

    for(int i = 0; i < list->uniformBuffers.size();i++)
    {
        destroyBuffer(list->uniformBuffers[i]);
    }
    
    for(int i = 0; i < list->models.size();i++)
    {
        delete list->models[i];
    }
    destroyDevice(list->device);
    delete list;
}
}
