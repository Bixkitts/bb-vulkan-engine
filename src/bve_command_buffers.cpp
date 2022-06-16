#include "bve_command_buffers.hpp"
#include "bve_swap_chain.hpp"

#include <stdexcept>
#include <array>

namespace bve
{

void createCommandBuffers(BveGraphicsPipeline *pipeline, std::vector<VkCommandBuffer>& commandBuffers, BveSwapChain *swapchain)
{
    commandBuffers.resize(swapchain->swapChainImages.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pipeline->myPipelineDevice->commandPool;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if(vkAllocateCommandBuffers(pipeline->myPipelineDevice->device_, &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (int i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }                

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.renderPass = swapchain->renderPass;
        renderPassInfo.framebuffer = swapchain->swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = swapchain->swapChainExtent;//make sure swapchain extent is being used here and notwindow extent

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        bindPipeline(pipeline, commandBuffers[i]);
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);//can set up instancing with this, take note for later

        vkCmdEndRenderPass(commandBuffers[i]);
        if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

}

}
