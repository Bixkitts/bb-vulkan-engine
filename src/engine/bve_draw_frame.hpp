#ifndef BVE_DRAW_FRAME
#define BVE_DRAW_FRAME

#include "bve_swap_chain.hpp"
#include "bve_buffers.hpp"
#include <glm/matrix.hpp>

#include <stdexcept>

namespace bve
{

void drawFrame(SwapChain* swapchain, std::vector<VkCommandBuffer> &commandBuffers, std::vector<UniformBuffer*> &uniformBuffers);
void updateUniformBuffer(uint32_t currentImage, SwapChain *swapchain, std::vector<UniformBuffer*> &uniformBuffers);

}
#endif
