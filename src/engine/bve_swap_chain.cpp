#include "bve_swap_chain.hpp"
#include "bve_device.hpp"
#include "bve_buffers.hpp"
#include "config_swap_chain.hpp"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace bve 
{

SwapChain* createSwapChain(Device* device, VkExtent2D extent)
{
    SwapChain* swapchain = new SwapChain;
    swapchain->device = device;
    initSwapChain(swapchain);
    createImageViews(swapchain);
    createRenderPass(swapchain);
    createDepthResources(swapchain);
    createFramebuffers(swapchain);
    createSyncObjects(swapchain);

    return swapchain;
}

float extentAspectRatio(SwapChain* swapchain)
{
return static_cast<float>(swapchain->swapChainExtent.width) 
    / static_cast<float>(swapchain->swapChainExtent.height);
}

void destroySwapchain(SwapChain* swapchain) 
{
    for (auto imageView : swapchain->swapChainImageViews) 
    {
        vkDestroyImageView(swapchain->device->logical, imageView, nullptr);
    }
    swapchain->swapChainImageViews.clear();

    if (swapchain->swapChain != nullptr) 
    {
        vkDestroySwapchainKHR(swapchain->device->logical, swapchain->swapChain, nullptr);
        swapchain->swapChain = nullptr;
    }

    for (int i = 0; i < swapchain->depthImages.size(); i++) 
    {
        vkDestroyImageView(swapchain->device->logical, swapchain->depthImageViews[i], nullptr);
        vkDestroyImage(swapchain->device->logical, swapchain->depthImages[i], nullptr);
        vkFreeMemory(swapchain->device->logical, swapchain->depthImageMemorys[i], nullptr);
    }

    for (auto framebuffer : swapchain->swapChainFramebuffers) 
    {
        vkDestroyFramebuffer(swapchain->device->logical, framebuffer, nullptr);
    }

    vkDestroyRenderPass(swapchain->device->logical, swapchain->renderPass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        vkDestroySemaphore(swapchain->device->logical, swapchain->renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(swapchain->device->logical, swapchain->imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(swapchain->device->logical, swapchain->inFlightFences[i], nullptr);
    }
    delete swapchain;
}

VkResult acquireNextImage(SwapChain* swapchain, uint32_t* imageIndex) 
{
    vkWaitForFences(
            swapchain->device->logical,
            1,
            &swapchain->inFlightFences[swapchain->currentFrame],
            VK_TRUE,
            std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
            swapchain->device->logical,
            swapchain->swapChain,
            std::numeric_limits<uint64_t>::max(),
            swapchain->imageAvailableSemaphores[swapchain->currentFrame],    // must be a not signaled semaphore
            VK_NULL_HANDLE,
            imageIndex);

    return result;
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

void initSwapChain(SwapChain* swapchain) 
{

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(swapchain->device->physical, swapchain->device);

    VkSurfaceFormatKHR surfaceFormat         = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode             = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent                        = chooseSwapExtent(swapchain, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) 
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    auto createInfo = config::createSwapchainInfo(swapchain, swapChainSupport, surfaceFormat, presentMode, extent, imageCount);

    if (vkCreateSwapchainKHR(swapchain->device->logical, &createInfo, nullptr, &swapchain->swapChain) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create swap chain!");
    }
    delete[] createInfo.pQueueFamilyIndices;

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(swapchain->device->logical, swapchain->swapChain, &imageCount, nullptr);
    swapchain->swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(swapchain->device->logical, swapchain->swapChain, &imageCount, swapchain->swapChainImages.data());

    swapchain->swapChainImageFormat = surfaceFormat.format;
    swapchain->swapChainExtent = extent;
}

void createImageViews(SwapChain* swapchain) 
{
    swapchain->swapChainImageViews.resize(swapchain->swapChainImages.size());
    for (size_t i = 0; i < swapchain->swapChainImages.size(); i++) 
    {
        auto viewInfo = config::createImageViewInfo(swapchain, i);

        if (vkCreateImageView(swapchain->device->logical, &viewInfo, nullptr, &swapchain->swapChainImageViews[i]) !=
                VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}


void createRenderPass(SwapChain* swapchain) 
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat(swapchain);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapchain->swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(swapchain->device->logical, &renderPassInfo, nullptr, &swapchain->renderPass) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void createFramebuffers(SwapChain* swapchain) 
{
    swapchain->swapChainFramebuffers.resize(swapchain->swapChainImages.size());
    for (size_t i = 0; i < swapchain->swapChainImages.size(); i++) 
    {
        std::array<VkImageView, 2> attachments = {swapchain->swapChainImageViews[i], swapchain->depthImageViews[i]};

        VkExtent2D swapChainExtent = swapchain->swapChainExtent;
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = swapchain->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
                        swapchain->device->logical,
                        &framebufferInfo,
                        nullptr,
                        &swapchain->swapChainFramebuffers[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void createDepthResources(SwapChain* swapchain) 
{
    VkFormat depthFormat = findDepthFormat(swapchain);
    VkExtent2D swapChainExtent = swapchain->swapChainExtent;

    swapchain->depthImages.resize(swapchain->swapChainImages.size());
    swapchain->depthImageMemorys.resize(swapchain->swapChainImages.size());
    swapchain->depthImageViews.resize(swapchain->swapChainImages.size());

    for (int i = 0; i < swapchain->depthImages.size(); i++) 
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        createImageWithInfo(
                imageInfo,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                swapchain->depthImages[i],
                swapchain->depthImageMemorys[i],
                swapchain->device);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchain->depthImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(swapchain->device->logical, &viewInfo, nullptr, &swapchain->depthImageViews[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

void createSyncObjects(SwapChain* swapchain) 
{
    swapchain->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    swapchain->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    swapchain->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    swapchain->imagesInFlight.resize(swapchain->swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        if (vkCreateSemaphore(swapchain->device->logical, &semaphoreInfo, nullptr, &swapchain->imageAvailableSemaphores[i]) !=
                        VK_SUCCESS ||
                vkCreateSemaphore(swapchain->device->logical, &semaphoreInfo, nullptr, &swapchain->renderFinishedSemaphores[i]) !=
                        VK_SUCCESS ||
                vkCreateFence(swapchain->device->logical, &fenceInfo, nullptr, &swapchain->inFlightFences[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats) 
{
    for (const auto &availableFormat : availableFormats) 
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes) 
{
    for (const auto &availablePresentMode : availablePresentModes) 
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    // for (const auto &availablePresentMode : availablePresentModes) {
    //     if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
    //         std::cout << "Present mode: Immediate" << std::endl;
    //         return availablePresentMode;
    //     }
    // }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(SwapChain* swapchain, const VkSurfaceCapabilitiesKHR &capabilities) 
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        return capabilities.currentExtent;
    } 
    else 
    {
        VkExtent2D actualExtent = swapchain->windowExtent;
        actualExtent.width = std::max(
                capabilities.minImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
                capabilities.minImageExtent.height,
                std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

VkFormat findDepthFormat(SwapChain* swapchain) 
{
    return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
            swapchain->device);
}

}    // namespace bve
