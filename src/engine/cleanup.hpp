#pragma once

#include "swap_chain.hpp"
#include "window.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "buffers.hpp"
struct CleanupList  //A list of objects that will need to have cleanup code called
{
    Device device;    
    GraphicsPipeline *pipeline;
    SwapChain *swapchain;
    std::vector<VertexBuffers*> vertexBuffers;
    std::vector<IndexBuffers*> indexBuffers;
    std::vector<UniformBuffers*> uniformBuffers;
    std::vector<Model*> models;
};

void cleanup(CleanupList* list);
