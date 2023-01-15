#include "bve_command_buffers.hpp"
#include "config_command_buffers.hpp"
#include "bve_buffer_vertex.hpp"
#include "bve_swap_chain.hpp"

#include <stdexcept>
#include <array>
#include <vulkan/vulkan_core.h>

namespace bve
{

    std::vector<VkCommandBuffer> createCommandBuffers(GraphicsPipeline* pipeline, SwapChain* swapchain, std::vector<VertexBuffer*> &vertexBuffers)
{
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(swapchain->swapChainImages.size());

    auto allocInfo = config::allocInfo(pipeline, commandBuffers);
    if(vkAllocateCommandBuffers(pipeline->device->logical, allocInfo, commandBuffers.data()) !=
            VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    delete allocInfo;

    for (int i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }                

        auto renderPassInfo = config::renderPassInfo(swapchain, i);
        vkCmdBeginRenderPass(commandBuffers[i], renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        //delete renderPassInfo;
        delete renderPassInfo->pClearValues;
        delete renderPassInfo;

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
