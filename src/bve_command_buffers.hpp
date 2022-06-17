#ifndef BVE_COMMAND_BUFFERS
#define BVE_COMMAND_BUFFERS

#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include "bve_model.hpp"

namespace bve{

void createCommandBuffers(BveGraphicsPipeline *pipeline, std::vector<VkCommandBuffer>& commandBuffers, BveSwapChain *swapchain, std::vector<BveModel*> models);

}
#endif
