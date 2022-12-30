#include "bve_command_buffers.hpp"
#include "bve_buffer_vertex.hpp"
#include "bve_swap_chain.hpp"

#include <stdexcept>
#include <array>
#include <vulkan/vulkan_core.h>

namespace bve
{

    std::vector<VkCommandBuffer> createCommandBuffers(GraphicsPipeline* pipeline, SwapChain* swapchain, std::vector<VertexBuffer*> vertexBuffers)
{
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(swapchain->swapChainImages.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pipeline->myPipelineDevice->commandPool;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if(vkAllocateCommandBuffers(pipeline->myPipelineDevice->logical, &allocInfo, commandBuffers.data()) !=
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
        #include "configs/command_buffers/renderPassInfo.conf"

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        bindPipeline(pipeline, commandBuffers[i]);

        bindVertexBuffer(vertexBuffers[0], commandBuffers[i]);
        drawVertexBuffer(vertexBuffers[0], commandBuffers[i]);

        vkCmdEndRenderPass(commandBuffers[i]);
        if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    return commandBuffers;

}

}
