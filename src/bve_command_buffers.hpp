#ifndef BVE_COMMAND_BUFFERS
#define BVE_COMMAND_BUFFERS

#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include "bve_model.hpp"

namespace bve{

void createCommandBuffers(GraphicsPipeline *pipeline, std::vector<VkCommandBuffer>& commandBuffers, SwapChain *swapchain, std::vector<Model*> models);

}
#endif
