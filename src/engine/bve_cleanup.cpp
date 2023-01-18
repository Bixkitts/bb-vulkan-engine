#include "bve_buffers.hpp"
#include "bve_pipeline.hpp"
#include "bve_window.hpp"
#include "bve_cleanup.hpp"
#include "bve_swap_chain.hpp"
#include "bve_device.hpp"
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
    destroyDevice(list->device);
}
}
