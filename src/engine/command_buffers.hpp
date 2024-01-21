#ifndef BVE_COMMAND_BUFFERS
#define BVE_COMMAND_BUFFERS

#include "swap_chain.hpp"
#include "entity.h"

typedef VkCommandBuffer *VkCommandBufferArray;

BBError         createPrimaryCommandBuffers (VkCommandBufferArray *commandBuffers, 
                                             const Device device);
BBError         recordPrimaryCommandBuffer  (VkCommandBuffer commandBuffer, 
                                             const RenderObjectArray entities, 
                                             const uint64_t entityCount, 
                                             const SwapChain swapchain,
                                             const VkFramebuffer frameBuffer);
VkResult        submitCommandBuffers        (SwapChain swapchain,
                                             const VkCommandBufferArray buffers, 
                                             uint32_t* imageIndex);
VkCommandBuffer beginSingleTimeCommands     (Device theGPU); 
void            endSingleTimeCommands       (VkCommandBuffer commandBuffer, 
                                             Device theGPU); 
#endif
