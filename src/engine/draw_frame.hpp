#ifndef BVE_DRAW_FRAME
#define BVE_DRAW_FRAME

#include "swap_chain.hpp"
#include "buffers.hpp"
#include "pipeline.hpp"
#include <glm/matrix.hpp>

#include <stdexcept>

namespace bve
{

void drawFrame(SwapChain* swapchain, GraphicsPipeline *pipeline, std::vector<VkCommandBuffer> &commandBuffers, std::vector<UniformBuffer*> &uniformBuffers, std::vector<VertexBuffer*> &vertexBuffers, std::vector<IndexBuffer*> &indexBuffers, std::vector<Model*> &models);
void updateUniformBuffer(uint32_t currentImage, SwapChain *swapchain, std::vector<UniformBuffer*> &uniformBuffers);

}
#endif
