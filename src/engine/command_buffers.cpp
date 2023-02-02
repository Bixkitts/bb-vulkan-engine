#include "command_buffers.hpp"
#include "config_command_buffers.hpp"
#include "buffers.hpp"
#include "swap_chain.hpp"

#include <stdexcept>
#include <array>
#include <vulkan/vulkan_core.h>

namespace bve
{

    std::vector<VkCommandBuffer> createCommandBuffers(GraphicsPipeline* pipeline, SwapChain* swapchain, std::vector<VertexBuffer*> &vertexBuffers, std::vector<IndexBuffer*> &indexBuffers)
{
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

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
        delete[] renderPassInfo->pClearValues;
        delete renderPassInfo;

        bindPipeline(pipeline, commandBuffers[i]);

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineConfig->pipelineLayout, 0, 1, &pipeline->pipelineConfig->descriptorSets[i], 0, nullptr);

        for(int j = 0; j < vertexBuffers.size(); j++)
        {
            bindVertexBuffer(vertexBuffers[j], commandBuffers[i]);
            bindIndexBuffer(indexBuffers[j], commandBuffers[i]);
            drawIndexBuffer(indexBuffers[j], commandBuffers[i]);
        }

        vkCmdEndRenderPass(commandBuffers[i]);
        if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
       }
    }
    return commandBuffers;

}
VkResult submitCommandBuffers(
                SwapChain* swapchain,
        const VkCommandBuffer* buffers, uint32_t* imageIndex) 
{
    if (swapchain->imagesInFlight[*imageIndex] != VK_NULL_HANDLE) 
    {
        vkWaitForFences(swapchain->device->logical, 1, &swapchain->imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    swapchain->imagesInFlight[*imageIndex] = swapchain->inFlightFences[swapchain->currentFrame];

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

    vkResetFences(swapchain->device->logical, 1, &swapchain->inFlightFences[swapchain->currentFrame]);
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
}
