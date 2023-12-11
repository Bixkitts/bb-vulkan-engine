#include "draw_frame.hpp"
#include "entity.h"
#include "error_handling.h"
#include "swap_chain.hpp"
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

BBError drawFrame(SwapChain swapchain, BBEntityArray entities, uint64_t entityCount)
{
    uint32_t imageIndex = 0;
    VkResult result     = acquireNextImage(swapchain, &imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        return BB_ERROR_ACQUIRE_SWAP_CHAIN_IMAGE;
    }

    vkResetCommandBuffer (commandBuffers[swapchain->currentFrame], 0);
    recordCommandBuffer  (commandBuffers[swapchain->currentFrame], 
                          pipeline, 
                          imageIndex, 
                          swapchain, 
                          vertexBuffers, 
                          indexBuffers, 
                          models);
    result = 
    submitCommandBuffers (swapchain, 
                          &commandBuffers[swapchain->currentFrame], 
                          &imageIndex);
    if (result != VK_SUCCESS){
        return BB_ERROR_COMMAND_BUFFER_RUN;
    }
    return BB_ERROR_OK;
}
