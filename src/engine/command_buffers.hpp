#ifndef BVE_COMMAND_BUFFERS
#define BVE_COMMAND_BUFFERS

#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

namespace bve{
    std::vector<VkCommandBuffer> createCommandBuffers(GraphicsPipeline* pipeline, SwapChain* swapchain, std::vector<VertexBuffer*> &vertexBuffers, std::vector<IndexBuffer*> &indexBuffers);
}
#endif
