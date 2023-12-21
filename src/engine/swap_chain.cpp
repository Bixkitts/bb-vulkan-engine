#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>

#include "swap_chain.hpp"
#include "buffers.hpp"
#include "defines.hpp"

static void initSwapChain             (SwapChain swapchain);
static void createSwapchainImageViews (SwapChain swapchain);
static void createDepthResources      (SwapChain swapchain);
static void createRenderPass          (SwapChain swapchain);
static void createFramebuffers        (SwapChain swapchain);
static void createSyncObjects         (SwapChain swapchain);

BBError createSwapChain(SwapChain *swapchain, 
                        const Device device, 
                        const VkExtent2D extent)
{
    //TODO: MALLOC without free()
    *swapchain           = (SwapChain)calloc(1,sizeof(SwapChain_T));
    if (*swapchain == NULL){
        return BB_ERROR_MEM;
    }

    (*swapchain)->device = device;

    initSwapChain             (*swapchain);
    createSwapchainImageViews (*swapchain);
    createRenderPass          (*swapchain);
    createDepthResources      (*swapchain);
    createFramebuffers        (*swapchain);
    createSyncObjects         (*swapchain);

    return BB_ERROR_OK;
}

float getExtentAspectRatio(SwapChain swapchain)
{
    return static_cast<float>(swapchain->swapChainExtent.width) 
    / static_cast<float>(swapchain->swapChainExtent.height);
}

void destroySwapchain(SwapChain swapchain) 
{
    for (auto imageView : swapchain->swapChainImageViews){
        vkDestroyImageView    (swapchain->device->logical, 
                               imageView, 
                               NULL);
    }
    swapchain->swapChainImageViews.clear();
    if (swapchain->swapChain != NULL) {
        vkDestroySwapchainKHR (swapchain->device->logical, 
                               swapchain->swapChain, 
                               NULL);
        swapchain->swapChain = NULL;
    }
    for (int i = 0; i < swapchain->depthImages.size(); i++) {
        // TODO: set pointers to NULL if it's not done by vulkan
        vkDestroyImageView    (swapchain->device->logical, 
                               swapchain->depthImageViews[i], 
                               NULL);
        vkDestroyImage        (swapchain->device->logical, 
                               swapchain->depthImages[i], 
                               NULL);
        vkFreeMemory          (swapchain->device->logical, 
                               swapchain->depthImageMemorys[i], 
                               NULL);
    }
    for (auto framebuffer : swapchain->framebuffers) {
        vkDestroyFramebuffer  (swapchain->device->logical, framebuffer, nullptr);
    }

    vkDestroyRenderPass       (swapchain->device->logical, swapchain->renderPass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore    (swapchain->device->logical, 
                               swapchain->renderFinishedSemaphores[i], 
                               NULL);
        vkDestroySemaphore    (swapchain->device->logical, 
                               swapchain->imageAvailableSemaphores[i], 
                               NULL);
        vkDestroyFence        (swapchain->device->logical, 
                               swapchain->inFlightFences[i], 
                               NULL);
    }
    delete swapchain;
}

VkResult acquireNextImage(SwapChain swapchain, uint32_t* imageIndex) 
{
    vkWaitForFences       (swapchain->device->logical,
                           1,
                           &swapchain->inFlightFences[swapchain->currentFrame],
                           VK_TRUE,
                           std::numeric_limits<uint64_t>::max());

    vkResetFences         (swapchain->device->logical, 
                           1, 
                           &swapchain->inFlightFences[swapchain->currentFrame]);

    VkResult result = 
    vkAcquireNextImageKHR (swapchain->device->logical,
                           swapchain->swapChain,
                           std::numeric_limits<uint64_t>::max(),
                           swapchain->imageAvailableSemaphores[swapchain->currentFrame],    // must be a not signaled semaphore
                           VK_NULL_HANDLE,
                           imageIndex);

    return result;
}

static void initSwapChain(SwapChain swapchain) 
{
    SwapChainSupportDetails swapChainSupport = 
    querySwapChainSupport   (swapchain->device->physical, swapchain->device);

    VkSurfaceFormatKHR      surfaceFormat    = 
    chooseSwapSurfaceFormat (swapChainSupport.formats);

    VkPresentModeKHR        presentMode      = 
    chooseSwapPresentMode   (swapChainSupport.presentModes);

    VkExtent2D              extent           = 
    chooseSwapExtent        (swapchain, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount){
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};

    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = swapchain->device->surface_;
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices  = findQueueFamilies(swapchain->device->physical, swapchain->device);

    uint32_t queueFamilyIndices[2];
    queueFamilyIndices[0]=indices.graphicsFamily;
    queueFamilyIndices[1]=indices.presentFamily;

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    } 
    else 
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;            // Optional
        createInfo.pQueueFamilyIndices   = NULL;    // Optional
    }

    createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;

    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(swapchain->device->logical, &createInfo, nullptr, &swapchain->swapChain) != VK_SUCCESS){
        throw std::runtime_error("failed to create swap chain!");
    }
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

static void createSwapchainImageViews(SwapChain swapchain) 
{
    swapchain->swapChainImageViews.resize(swapchain->swapChainImages.size());
    for (size_t i = 0; i < swapchain->swapChainImages.size(); i++) 
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = swapchain->swapChainImages[i];
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = swapchain->swapChainImageFormat;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(swapchain->device->logical, &viewInfo, nullptr, &swapchain->swapChainImageViews[i]) 
            != VK_SUCCESS){
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

static void createRenderPass(SwapChain swapchain) 
{
    VkSubpassDependency     dependency         = {};
    VkAttachmentDescription depthAttachment    = {};
    VkAttachmentReference   depthAttachmentRef = {};
    VkAttachmentDescription colorAttachment    = {};
    VkAttachmentReference   colorAttachmentRef = {};
    VkSubpassDescription    subpass            = {};
    VkRenderPassCreateInfo  renderPassInfo     = {};
    std::array
    <VkAttachmentDescription, 2> attachments; 

    depthAttachment.format          = findDepthFormat(swapchain);
    depthAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    depthAttachmentRef.attachment   = 1;
    depthAttachmentRef.layout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    colorAttachment.format          = swapchain->swapChainImageFormat;
    colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    colorAttachmentRef.attachment   = 0;
    colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    dependency.srcSubpass           = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask        = 0;
    dependency.srcStageMask         =
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
    | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass           = 0;
    dependency.dstStageMask         =
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
    | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask        =
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT 
    | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    attachments                    = {colorAttachment, depthAttachment};

    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments    = attachments.data();
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    if (vkCreateRenderPass(swapchain->device->logical, 
                           &renderPassInfo, 
                           NULL, 
                           &swapchain->renderPass) 
        != VK_SUCCESS){
        throw std::runtime_error("failed to create render pass!");
    }
}

static void createFramebuffers(SwapChain swapchain) 
{
    VkExtent2D              swapChainExtent;
    VkFramebufferCreateInfo framebufferInfo;
    swapchain->framebuffers.resize(swapchain->swapChainImages.size());
    for (size_t i = 0; i < swapchain->swapChainImages.size(); i++) 
    {
        // TODO: fucking disgusting
        std::array<VkImageView, 2> attachments = {swapchain->swapChainImageViews[i], swapchain->depthImageViews[i]};

        swapChainExtent                  = swapchain->swapChainExtent;
        framebufferInfo                  = {};
        framebufferInfo.sType            = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass       = swapchain->renderPass;
        framebufferInfo.attachmentCount  = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments     = attachments.data();
        framebufferInfo.width            = swapChainExtent.width;
        framebufferInfo.height           = swapChainExtent.height;
        framebufferInfo.layers           = 1;

        if (vkCreateFramebuffer(
                        swapchain->device->logical,
                        &framebufferInfo,
                        nullptr,
                        &swapchain->framebuffers[i]) 
            != VK_SUCCESS){
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

static void createDepthResources(SwapChain swapchain) 
{
    VkFormat          depthFormat     = findDepthFormat(swapchain);
    VkExtent2D        swapChainExtent = swapchain->swapChainExtent;
    VkImageCreateInfo imageInfo{};

    // TODO: stdlib shit
    swapchain->depthImages.resize       (swapchain->swapChainImages.size());
    swapchain->depthImageMemorys.resize (swapchain->swapChainImages.size());
    swapchain->depthImageViews.resize   (swapchain->swapChainImages.size());

    for (int i = 0; i < swapchain->depthImages.size(); i++) 
    {
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = depthFormat;
        imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags         = 0;

        createImageWithInfo(imageInfo,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            swapchain->depthImages[i],
                            swapchain->depthImageMemorys[i],
                            swapchain->device);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = swapchain->depthImages[i];
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = depthFormat;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(swapchain->device->logical, 
                              &viewInfo, 
                              NULL, 
                              &swapchain->depthImageViews[i]) 
            != VK_SUCCESS){
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

static void createSyncObjects(SwapChain swapchain) 
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    VkFenceCreateInfo     fenceInfo     = {};

    swapchain->imageAvailableSemaphores.resize (MAX_FRAMES_IN_FLIGHT);
    swapchain->renderFinishedSemaphores.resize (MAX_FRAMES_IN_FLIGHT);
    swapchain->inFlightFences.resize           (MAX_FRAMES_IN_FLIGHT);

    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    fenceInfo.sType     = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags     = VK_FENCE_CREATE_SIGNALED_BIT;

    //TODO: what the fuck
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        if (vkCreateSemaphore(swapchain->device->logical, 
                              &semaphoreInfo, 
                              NULL, 
                              &swapchain->imageAvailableSemaphores[i]) 
            != VK_SUCCESS 
            || 
            vkCreateSemaphore(swapchain->device->logical, 
                              &semaphoreInfo, 
                              NULL, 
                              &swapchain->renderFinishedSemaphores[i]) 
            != VK_SUCCESS 
            ||
            vkCreateFence    (swapchain->device->logical, 
                              &fenceInfo, 
                              NULL, 
                              &swapchain->inFlightFences[i]) 
            != VK_SUCCESS){
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats){
    for (const auto &availableFormat : availableFormats){
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) 
{
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR){
            printf("Present Mode: Mailbox\n");
            return availablePresentMode;
        }
    }
    // for (const auto &availablePresentMode : availablePresentModes) {
    //     if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
    //         std::cout << "Present mode: Immediate" << std::endl;
    //         return availablePresentMode;
    //     }
    // }
    printf("Present Mode: V-Sync\n");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(SwapChain swapchain, const VkSurfaceCapabilitiesKHR capabilities) 
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

VkFormat findDepthFormat(SwapChain swapchain) 
{
    return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
            swapchain->device);
}
