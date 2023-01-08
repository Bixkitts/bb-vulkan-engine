#ifndef BVE_DRAW_FRAME
#define BVE_DRAW_FRAME

#include "bve_swap_chain.hpp"

#include <stdexcept>

namespace bve
{

void drawFrame(SwapChain *swapchain, std::vector<VkCommandBuffer> &commandBuffers);

}
#endif
