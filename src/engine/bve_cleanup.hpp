#pragma once

#include "bve_swap_chain.hpp"
#include "bve_window.hpp"
#include "bve_device.hpp"
#include "bve_pipeline.hpp"
#include "bve_buffers.hpp"
namespace bve
{ 
    struct CleanupList  //A list of objects that will need to have cleanup code called
    {
        Device *device;    
        GraphicsPipeline *pipeline;
        SwapChain *swapchain;
        std::vector<VertexBuffer*> vertexBuffers;
        std::vector<VertexBuffer*> indexBuffers;
        std::vector<Model*> models;
    };

    void cleanup(CleanupList* list);
}
