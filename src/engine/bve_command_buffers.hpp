#ifndef BVE_COMMAND_BUFFERS
#define BVE_COMMAND_BUFFERS

#include "bve_buffers.hpp"
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include "bve_model.hpp"

namespace bve{
    std::vector<VkCommandBuffer> createCommandBuffers(GraphicsPipeline* pipeline, SwapChain* swapchain, std::vector<VertexBuffer*> &vertexBuffers, std::vector<IndexBuffer*> &indexBuffers);
}
#endif
