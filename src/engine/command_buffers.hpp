#ifndef BVE_COMMAND_BUFFERS
#define BVE_COMMAND_BUFFERS

#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

std::vector<VkCommandBuffer> createCommandBuffers(GraphicsPipeline* pipeline);

void recordCommandBuffer(VkCommandBuffer commandBuffer, 
                        GraphicsPipeline *pipeline, 
                        uint32_t imageIndex, 
                        SwapChain* swapchain, 
                        std::vector<VertexBuffers*> &vertexBuffers, 
                        std::vector<IndexBuffers*> &indexBuffers,
                        std::vector<Model*> &models);

VkResult submitCommandBuffers(
            SwapChain* swapchain,
            const VkCommandBuffer* buffers, uint32_t* imageIndex) ;
//single time commands
VkCommandBuffer beginSingleTimeCommands(Device theGPU); 
void endSingleTimeCommands(VkCommandBuffer commandBuffer, Device theGPU); 
#endif
