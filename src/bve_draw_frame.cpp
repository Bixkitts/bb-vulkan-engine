#include "bve_draw_frame.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>

namespace bve
{

void drawFrame(SwapChain* swapchain, std::vector<VkCommandBuffer> commandBuffers)
{
    uint32_t imageIndex;
    auto result = acquireNextImage(swapchain, &imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    result = submitCommandBuffers(swapchain, &commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to run command from command buffer!");
    }

}

}
