#ifndef CONFIG_COMMAND_BUFFERS
#define CONFIG_COMMAND_BUFFERS
#include "pipeline.hpp"
#include <vulkan/vulkan_core.h>
namespace config
{
    VkRenderPassBeginInfo *renderPassInfo(bve::SwapChain *swapchain, int framebufferIndex);
    VkCommandBufferAllocateInfo* allocInfo(bve::GraphicsPipeline* pipeline, std::vector<VkCommandBuffer> &commandBuffers);
}

#endif
