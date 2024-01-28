#include "images.hpp"
#include "device.hpp"
#include "error_handling.h"
#include "command_buffers.hpp"
#include <vulkan/vulkan_core.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct VulkanImage_T
{
    Device         device;
    VkFormat       format;
    VkImage        imageHandle;
    VkDeviceMemory deviceMemory;
    VkImageView    views    [IMAGE_VIEW_COUNT];
    VkSampler      samplers [IMAGE_SAMPLER_COUNT];
    // Maybe I can get these from VkImage or smthn??
    VkExtent2D     extent;
    uint32_t       layerCount;
};

static void     copyBufferToImage     (VulkanBuffer buffer, 
                                       VulkanImage_T *image);
static BBError  initVulkanImage       (VulkanImage_T *image,
                                       VkImageCreateInfo *createInfo,
                                       VkMemoryPropertyFlags memProperties,
                                       Device device);
static VkFormat findDepthImageFormat  (Device device); 

// to get my memory management together!
BBError createTextureImage (VulkanImage_T **image, 
                            const char *dir, 
                            Device device)
{
    VkDevice           logicalDevice       = getLogicalDevice(device);
    void              *data                = NULL;
    BBError            er                  = BB_ERROR_UNKNOWN;
    int                texWidth            = 0;
    int                texHeight           = 0; 
    int                texChannels         = 0;
    VkDeviceSize       imageSize           = {0};
    StagingBuffer      sBuffer             = NULL;
    VkDeviceMemory     stagingBufferMemory = {0};
    VkImageCreateInfo  imageCreateInfo     = {};

    // TODO: custom image loader?
    stbi_uc           *pixels              = stbi_load(dir, 
                                                       &texWidth, 
                                                       &texHeight, 
                                                       &texChannels, 
                                                       STBI_rgb_alpha);
    if(!pixels){
        return BB_ERROR_IMAGE_CREATE;
    }

    // TODO: magic number 4
    imageSize = texWidth * texHeight * 4;
    //TODO: MALLOC without free
    *image    = (VulkanImage_T*)calloc(1, sizeof(VulkanImage_T));
    if (image == NULL){
        return BB_ERROR_MEM;
    }

    imageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width  = texWidth;
    imageCreateInfo.extent.height = texHeight;
    imageCreateInfo.extent.depth  = 1;
    imageCreateInfo.mipLevels     = 1;
    imageCreateInfo.arrayLayers   = 1;
    imageCreateInfo.format        = (*image)->format;
    imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.flags         = 0; // Optional


    createStagingBuffer   (&sBuffer, device, imageSize);
    copyIntoStagingBuffer (sBuffer, pixels, imageSize);
    stbi_image_free       (pixels);
    er = 
    initVulkanImage       (*image,
                           &imageCreateInfo,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           device);
    if (er != BB_ERROR_OK){
        goto error_exit;
    }
    transitionImageLayout ((*image), 
                           VK_IMAGE_LAYOUT_UNDEFINED, 
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage     (sBuffer, 
                           (*image));
    transitionImageLayout ((*image), 
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    
    destroyBuffer         (&sBuffer);
    
    return BB_ERROR_OK;

error_exit:
    free(*image);
    *image = NULL;
    return er;
}

BBError createSwapchainImages(VulkanImage_T *images[], 
                              Device device, 
                              VkSwapchainKHR swapchain,
                              uint32_t *count)
{
    VkDevice  logicalDevice = getLogicalDevice(device);
    VkImage  *swapchainImages = NULL;
    vkGetSwapchainImagesKHR(logicalDevice, 
                            swapchain, 
                            count, 
                            NULL);
    *images = (VulkanImage_T*)calloc((*count), sizeof(VulkanImage_T));
    if (*images == NULL) {
        return BB_ERROR_MEM;
    }
    swapchainImages = (VkImage*)malloc((*count) * sizeof(VkImage));
    if (swapchainImages == NULL) {
        free(*images);
        return BB_ERROR_MEM;
    }
    for (int i = 0; i < (*count); i++) {
        swapchainImages[i] = (*images)[i].imageHandle;
    }

    vkGetSwapchainImagesKHR(logicalDevice, swapchain, count, swapchainImages);

    swapchain->swapChainImageFormat = surfaceFormat.format;
    swapchain->swapChainExtent = extent;

}

BBError createSwapchainImageViews(VulkanImage_T* swapchainImages[], 
                                  uint32_t imageCount) 
{
    VkImageViewCreateInfo createInfo      = {};
    VkDevice              logicalDevice = getLogicalDevice(swapchainImages[0]->device);
    createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel   = 0;
    createInfo.subresourceRange.levelCount     = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount     = 1;

    for (size_t i = 0; i < imageCount; i++) 
    {
        createInfo.image  = swapchainImages[i]->imageHandle;
        createInfo.format = swapchainImages[i]->format;

        if (vkCreateImageView(logicalDevice, 
                              &createInfo, 
                              NULL, 
                              &swapchainImages[i]->imageHandle) 
            != VK_SUCCESS){
            return BB_ERROR_IMAGE_VIEW_CREATE;
        }
    }
    return BB_ERROR_OK;
}
BBError createDepthImage(VulkanImage_T **image, VkExtent2D extent)
{
    
    VkImageCreateInfo createInfo = {};
    createInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType     = VK_IMAGE_TYPE_2D;
    createInfo.extent.width  = extent.width;
    createInfo.extent.height = extent.height;
    createInfo.extent.depth  = 1;
    createInfo.mipLevels     = 1;
    createInfo.arrayLayers   = 1;
    createInfo.format        = findDepthImageFormat((*image)->device);
    createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    createInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    createInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.flags         = 0;

    return BB_ERROR_OK;
}

static VkFormat findDepthImageFormat(Device device) 
{
    return findSupportedFormat({VK_FORMAT_D32_SFLOAT, 
                                VK_FORMAT_D32_SFLOAT_S8_UINT, 
                                VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
                               device);
}

void destroyImage(VulkanImage_T **v)
{
    VkDevice logicalDevice = getLogicalDevice((*v)->device);
    if (*v == NULL){
        return;
    }
    // We're destroying these in reverse because.... ???
    for(uint64_t i = IMAGE_VIEW_COUNT - 1; i >= 0; i--){
        if ((*v)->views[i] != NULL) {
            vkDestroyImageView (logicalDevice, 
                                (*v)->views[i], 
                                NULL);
        }
    }
    for(uint64_t i = IMAGE_SAMPLER_COUNT - 1; i >= 0; i--){
        if ((*v)->samplers[i] != NULL) {
            vkDestroySampler   (logicalDevice, 
                                (*v)->samplers[i], 
                                NULL);
        }
    }
    vkDestroyImage (logicalDevice, 
                    (*v)->imageHandle, 
                    NULL);
    vkFreeMemory   (logicalDevice, 
                    (*v)->deviceMemory, 
                    NULL);
    free           (*v);
    *v = NULL;
}

static BBError initVulkanImage(VulkanImage_T *image,
                               VkImageCreateInfo *createInfo,
                               VkMemoryPropertyFlags memProperties,
                               Device device)
{
    VkMemoryAllocateInfo allocInfo       = {};
    VkMemoryRequirements memRequirements = {};
    VkDevice             logicalDevice   = getLogicalDevice(device);

    (image)->device        = device;
    (image)->format        = VK_FORMAT_R8G8B8A8_SRGB;
    (image)->extent.width  = createInfo->extent.width;
    (image)->extent.height = createInfo->extent.height;
    // TODO: not sure if this is the correct thing I want to track!
    (image)->layerCount    = createInfo->arrayLayers; // TODO: placeholder + sensitive invariant
                                                   //
    if (vkCreateImage(logicalDevice, createInfo, nullptr, &image->imageHandle) != VK_SUCCESS) {
        return BB_ERROR_IMAGE_CREATE;
    }
    vkGetImageMemoryRequirements (logicalDevice, image->imageHandle, &memRequirements);

    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memProperties, device);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &image->deviceMemory) != VK_SUCCESS) {
        return BB_ERROR_MEM;
    }

    vkBindImageMemory            (logicalDevice, image->imageHandle, image->deviceMemory, 0);
    return BB_ERROR_OK;
}

BBError transitionImageLayout(VulkanImage_T *image, 
                              VkImageLayout oldLayout, 
                              VkImageLayout newLayout)
{
    VkCommandBuffer      commandBuffer     = beginSingleTimeCommands(image->device);
    VkPipelineStageFlags sourceStage       = {};
    VkPipelineStageFlags destinationStage  = {};
    VkImageMemoryBarrier barrier           = {};

    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image->imageHandle;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.srcAccessMask                   = 0; // TODO
    barrier.dstAccessMask                   = 0; // TODO

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
    && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
    && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } 
    else{
        return BB_ERROR_IMAGE_LAYOUT_TRANSITION;
    }

    vkCmdPipelineBarrier(commandBuffer,
                         0 /* TODO */, 
                         0 /* TODO */,
                         0,
                         0, NULL,
                         0, NULL,
                         1, &barrier);

    endSingleTimeCommands(&commandBuffer, image->device);
    return BB_ERROR_OK;
}

VkImage     getImageHandle         (VulkanImage image)
{
    return image->imageHandle;
}
BBError createTextureImageView(VulkanImage image, TextureImageViewType type)
{
    VkDevice              logicalDevice = getLogicalDevice(image->device);
    VkImageViewCreateInfo viewInfo      = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image->imageHandle;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = image->format;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(logicalDevice, 
                          &viewInfo, 
                          NULL, 
                          &image->views[type]) 
        != VK_SUCCESS) {
        return BB_ERROR_IMAGE_VIEW_CREATE;
    }
    else{
        return BB_ERROR_IMAGE_VIEW_CREATE;
    }
    return BB_ERROR_OK;
}

BBError createDepthImageView(VulkanImage image, DepthImageViewType type)
{
return BB_ERROR_OK;
}

BBError createTextureSampler(VulkanImage image, ImageSamplerType type)
{
    VkDevice            logicalDevice = getLogicalDevice(image->device);
    VkSamplerCreateInfo samplerInfo   = {};
    samplerInfo.sType                    = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter                = VK_FILTER_LINEAR;
    samplerInfo.minFilter                = VK_FILTER_LINEAR;
    samplerInfo.addressModeU             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable         = VK_TRUE;
    samplerInfo.maxAnisotropy            = (getDevPhysicalProperties(image->device)).limits.maxSamplerAnisotropy;
    samplerInfo.borderColor              = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates  = VK_FALSE;
    samplerInfo.compareEnable            = VK_FALSE;
    samplerInfo.compareOp                = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode               = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias               = 0.0f;
    samplerInfo.minLod                   = 0.0f;
    samplerInfo.maxLod                   = 0.0f;
    //TODO: stdlib shit
    if (vkCreateSampler(logicalDevice, 
                        &samplerInfo, 
                        NULL, 
                        &image->samplers[type]) 
    != VK_SUCCESS){
        return BB_ERROR_IMAGE_SAMPLER_CREATE;
    }
    else{
        return BB_ERROR_IMAGE_SAMPLER_CREATE;
    }
    return BB_ERROR_OK;
}

VkImageView getTextureImageView    (VulkanImage_T *image,
                                    ImageViewType viewType)
{
    return image->views[viewType];
}

VkSampler   getTextureImageSampler (VulkanImage_T *image,
                                    ImageSamplerType samplerType)
{
    return image->samplers[samplerType];
}

static void copyBufferToImage(VulkanBuffer_T *buffer, 
                              VulkanImage_T *image)                       
{
    VkCommandBuffer commandBuffer = 
    beginSingleTimeCommands (image->device);

    VkBuffer        srcBuffer     =
    getVkBuffer             (buffer);
  
    VkBufferImageCopy region = {0};
    region.bufferOffset                    = 0;
    region.bufferRowLength                 = 0;
    region.bufferImageHeight               = 0;
  
    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = image->layerCount;
  
    region.imageOffset                     = {0, 0, 0};
    region.imageExtent                     = {image->width, image->height, 1};
  
    vkCmdCopyBufferToImage (commandBuffer,
                            srcBuffer,
                            image->imageHandle,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            1,
                            &region);
    endSingleTimeCommands  (&commandBuffer, 
                            image->device);
}
