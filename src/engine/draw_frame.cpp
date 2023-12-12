#include "draw_frame.hpp"
#include "entity.h"
#include "error_handling.h"
#include "swap_chain.hpp"
#include "command_buffers.hpp"
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

BBError drawFrame(const SwapChain swapchain, 
                  const VkCommandBufferArray commandBuffers,
                  const BBEntityArray entities, 
                  const uint64_t entityCount)
{
    uint32_t imageIndex = 0;
    VkResult result;

    result =
    acquireNextImage           (swapchain, &imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        return BB_ERROR_ACQUIRE_SWAP_CHAIN_IMAGE;
    }
    vkResetCommandBuffer       (commandBuffers[swapchain->currentFrame], 0);
    recordPrimaryCommandBuffer (commandBuffers[swapchain->currentFrame], 
                                entities,
                                entityCount,
                                swapchain,
                                swapchain->framebuffers[swapchain->currentFrame]);
    result = 
    submitCommandBuffers       (swapchain, 
                                &commandBuffers[swapchain->currentFrame], 
                                &imageIndex);
    if (result != VK_SUCCESS){
        return BB_ERROR_COMMAND_BUFFER_RUN;
    }
    return BB_ERROR_OK;
}
