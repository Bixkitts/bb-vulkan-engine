#ifndef BVE_DRAW_FRAME
#define BVE_DRAW_FRAME

#include <stdexcept>
#include <glm/matrix.hpp>

#include "swap_chain.hpp"
#include "buffers.hpp"
#include "pipeline.hpp"

// TODO: stdlib shit... uuuurgh
void drawFrame           (SwapChain* swapchain, 
                          GraphicsPipeline *pipeline, 
                          std::vector<VkCommandBuffer> &commandBuffers, 
                          std::vector<UniformBuffers*> &uniformBuffers, 
                          std::vector<VertexBuffers*> &vertexBuffers, 
                          std::vector<IndexBuffers*> &indexBuffers, 
                          std::vector<Model*> &models);
void updateUniformBuffer (uint32_t currentImage, 
                          SwapChain *swapchain, 
                          std::vector<UniformBuffers*> &uniformBuffers);

#endif
