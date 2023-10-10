#include "command_buffers.hpp"
#include "buffers.hpp"
#include "model.hpp"
#include "swap_chain.hpp"

#include <stdexcept>
#include <array>
#include <vulkan/vulkan_core.h>

namespace bve
{

    std::vector<VkCommandBuffer> createCommandBuffers(GraphicsPipeline* pipeline)
{
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pipeline->device->commandPool;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if(vkAllocateCommandBuffers(pipeline->device->logical, &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    return commandBuffers;

}

void recordCommandBuffer(VkCommandBuffer commandBuffer, GraphicsPipeline *pipeline, uint32_t imageIndex, SwapChain* swapchain, std::vector<VertexBuffer*> &vertexBuffers, std::vector<IndexBuffer*> &indexBuffers, std::vector<Model*> &models)
{

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }                

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain->renderPass;
        renderPassInfo.framebuffer = swapchain->swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = swapchain->swapChainExtent;

        auto clearValues = new VkClearValue[2];
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        bindPipeline(pipeline, commandBuffer);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineConfig->pipelineLayout, 0, 1, &pipeline->pipelineConfig->descriptorSets[swapchain->currentFrame], 0, nullptr);

        for(int j = 0; j < vertexBuffers.size(); j++)
        {
            bindVertexBuffer(vertexBuffers[j], commandBuffer);
            bindIndexBuffer(indexBuffers[j], commandBuffer);
            drawIndexBuffer(indexBuffers[j], commandBuffer);
        }

        vkCmdEndRenderPass(commandBuffer);
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
       }
    }

VkResult submitCommandBuffers(SwapChain *swapchain,
        const VkCommandBuffer* buffers, uint32_t* imageIndex) 
{

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {swapchain->imageAvailableSemaphores[swapchain->currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    VkSemaphore signalSemaphores[] = {swapchain->renderFinishedSemaphores[swapchain->currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(swapchain->device->graphicsQueue_, 1, &submitInfo, swapchain->inFlightFences[swapchain->currentFrame]) !=
            VK_SUCCESS) 
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(swapchain->device->presentQueue_, &presentInfo);

    swapchain->currentFrame = (swapchain->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

VkCommandBuffer beginSingleTimeCommands(Device* theGPU) 
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = theGPU->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(theGPU->logical, &allocInfo, &commandBuffer);
  
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer, Device* theGPU) 
{
    vkEndCommandBuffer(commandBuffer);
  
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
  
    vkQueueSubmit(theGPU->graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(theGPU->graphicsQueue_);
  
    vkFreeCommandBuffers(theGPU->logical, theGPU->commandPool, 1, &commandBuffer);
}
}
