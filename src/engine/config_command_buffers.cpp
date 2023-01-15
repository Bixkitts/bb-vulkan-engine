#include "config_command_buffers.hpp"
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include <vulkan/vulkan_core.h>
#include <array>

namespace config
{
    VkRenderPassBeginInfo *renderPassInfo(bve::SwapChain *swapchain, int framebufferIndex)
    {
        auto renderPassInfo = new VkRenderPassBeginInfo{};
        renderPassInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo->renderPass = swapchain->renderPass;
        renderPassInfo->framebuffer = swapchain->swapChainFramebuffers[framebufferIndex];
        renderPassInfo->renderArea.offset = {0,0};
        renderPassInfo->renderArea.extent = swapchain->swapChainExtent;

        auto clearValues = new VkClearValue[2];
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo->clearValueCount = 2;
        renderPassInfo->pClearValues = clearValues;
        return renderPassInfo;
    }                                                         
    VkCommandBufferAllocateInfo* allocInfo(bve::GraphicsPipeline* pipeline, std::vector<VkCommandBuffer> &commandBuffers)
    {
        auto allocInfo = new VkCommandBufferAllocateInfo{};
        allocInfo->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo->level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo->commandPool = pipeline->device->commandPool;
        allocInfo->commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        return allocInfo;
    }
}
