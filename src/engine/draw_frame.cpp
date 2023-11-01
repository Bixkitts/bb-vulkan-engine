#include <vector>
#include <chrono>

#include "draw_frame.hpp"
#include "entity.hpp"
#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "command_buffers.hpp"
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void drawFrame(SwapChain* swapchain, GraphicsPipeline *pipeline, BBEntity *entities, uint64_t entityCount)
{
    uint32_t imageIndex;
    auto result = acquireNextImage(swapchain, &imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetCommandBuffer(commandBuffers[swapchain->currentFrame], 0);

    recordCommandBuffer(commandBuffers[swapchain->currentFrame], pipeline, imageIndex, swapchain, vertexBuffers, indexBuffers, models);


    result = submitCommandBuffers(swapchain, &commandBuffers[swapchain->currentFrame], &imageIndex);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to run command from command buffer!");
    }
}
