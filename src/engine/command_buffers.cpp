#include <vulkan/vulkan_core.h>

#include "command_buffers.hpp"
#include "buffers.hpp"
#include "error_handling.h"
#include "swap_chain.hpp"
#include "entity.h"

BBError createPrimaryCommandBuffers(VkCommandBufferArray *commandBuffers, Device device)
{
    *commandBuffers = (VkCommandBufferArray)calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkCommandBuffer));
    if (commandBuffers == NULL){
        return BB_ERROR_MEM;
    }

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = getDevCommandPool(device);
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(getLogicalDevice(device), &allocInfo, *commandBuffers) !=
        VK_SUCCESS){
        return BB_ERROR_COMMAND_BUFFER_CREATE;
    }

    return BB_ERROR_OK;
}

BBError recordPrimaryCommandBuffer(VkCommandBuffer commandBuffer, 
                                   const RenderObjectArray entities, 
                                   const uint64_t entityCount, 
                                   const SwapChain swapchain,
                                   const VkFramebuffer frameBuffer)
{
    VkRenderPassBeginInfo    renderPassInfo = {};
    VkCommandBufferBeginInfo beginInfo      = {};
    GraphicsPipeline         pipelineToBind = {0};

    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(commandBuffer, 
                             &beginInfo) 
        != VK_SUCCESS){
        return BB_ERROR_COMMAND_BUFFER_RECORD;
    }                

    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = swapchain->renderPass;
    renderPassInfo.framebuffer       = frameBuffer;
    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = swapchain->swapChainExtent;

    // TODO: This is fucked fix this as soon as this program is running
    auto clearValues                 = new VkClearValue[2];
    clearValues[0].color             = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil      = {1.0f, 0};
    renderPassInfo.clearValueCount   = 2;
    renderPassInfo.pClearValues      = clearValues;

    // -------------------------- Start of Render Pass ---------------------------

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // I should be building secondary command buffers on a per object basis
    // And bind all their own pipelines.
    // ACTUALLY maybe secondary buffers on a per-pipeline basis!
    for(uint64_t i = 0; i < entityCount; i++){
        pipelineToBind = entities[i]->pipeline;
        bindPipeline            (pipelineToBind, commandBuffer);
        vkCmdBindDescriptorSets (commandBuffer, 
                                 VK_PIPELINE_BIND_POINT_GRAPHICS, 
                                 getLayout(pipelineToBind), 
                                 0, 
                                 1, 
                                 &getPipelineDescriptorSets(pipelineToBind)[swapchain->currentFrame], 
                                 0, 
                                 NULL);
        bindVertexBuffer        (entities[i]->vBuffer, commandBuffer);
        bindIndexBuffer         (entities[i]->iBuffer, commandBuffer);
        drawIndexBuffer         (entities[i]->iBuffer, commandBuffer);
    }
    // -------------------------- End of Render Pass ---------------------------
    vkCmdEndRenderPass(commandBuffer);
    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        return BB_ERROR_COMMAND_BUFFER_RECORD;
    }
    return BB_ERROR_OK;
}

VkResult submitCommandBuffers(const SwapChain swapchain,
                              const VkCommandBufferArray buffers, 
                              uint32_t* imageIndex) 
{
    VkSubmitInfo         submitInfo         = {};
    VkSemaphore          waitSemaphores[]   = 
                         {swapchain->imageAvailableSemaphores[swapchain->currentFrame]};
    VkPipelineStageFlags waitStages[]       = 
                         {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSwapchainKHR       swapChains[]       = 
                         {swapchain->swapChain};
    VkSemaphore          signalSemaphores[] = 
                         {swapchain->renderFinishedSemaphores[swapchain->currentFrame]};
    VkPresentInfoKHR     presentInfo        = {};
    VkResult             result             = VK_ERROR_UNKNOWN;

    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = buffers;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    result = 
    vkQueueSubmit(getDevGraphicsQueue(swapchain->device), 
                  1, 
                  &submitInfo, 
                  swapchain->inFlightFences[swapchain->currentFrame]);
    if (result != VK_SUCCESS){
        return result;
    }

    presentInfo.sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount  = 1;
    presentInfo.pWaitSemaphores     = signalSemaphores;
    presentInfo.swapchainCount      = 1;
    presentInfo.pSwapchains         = swapChains;
    presentInfo.pImageIndices       = imageIndex;

    result = 
    vkQueuePresentKHR(getDevPresentQueue(swapchain->device), 
                      &presentInfo);

    swapchain->currentFrame = (swapchain->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

VkCommandBuffer beginSingleTimeCommands(Device device) 
{
    VkCommandBufferAllocateInfo allocInfo     = {};
    VkCommandBuffer             commandBuffer = {};
    VkCommandBufferBeginInfo    beginInfo     = {};

    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = getDevCommandPool(device);
    allocInfo.commandBufferCount = 1;

    beginInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags              = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkAllocateCommandBuffers (getLogicalDevice(device), &allocInfo, &commandBuffer);
    vkBeginCommandBuffer     (commandBuffer, &beginInfo);

    return commandBuffer;
}

// TODO: This might be broken
void endSingleTimeCommands(VkCommandBuffer *commandBuffer, Device device) 
{
    VkSubmitInfo submitInfo = {};

    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = commandBuffer;

    vkEndCommandBuffer   (*commandBuffer);
    vkQueueSubmit        (getDevGraphicsQueue(device), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle      (getDevGraphicsQueue(device));
    vkFreeCommandBuffers (getLogicalDevice(device), getDevCommandPool(device), 1, commandBuffer);
}
