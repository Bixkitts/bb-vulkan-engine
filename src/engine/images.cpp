#include "images.hpp"
#include "device.hpp"
#include "error_handling.h"
#include "command_buffers.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct VulkanImage_T
{
    Device         device;
    VkFormat       format;
    VkImage        textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView    views    [IMAGE_VIEW_COUNT];
    VkSampler      samplers [IMAGE_SAMPLER_COUNT];
    // Maybe I can get these from VkImage or smthn??
    uint32_t       width;
    uint32_t       height;
    uint32_t       layerCount;
};

static void    copyBufferToImage (VulkanBuffer buffer, 
                                  VulkanImage_T *image);
// TODO: too many params
static BBError createImage       (const uint32_t width, 
                                  const uint32_t height, 
                                  const VkFormat format, 
                                  const VkImageTiling tiling, 
                                  const VkImageUsageFlags usage, 
                                  const VkMemoryPropertyFlags properties, 
                                  VkImage image, 
                                  VkDeviceMemory imageMemory, 
                                  const Device device);

// to get my memory management together!
BBError createTextureImage (VulkanImage_T **image, 
                            const char *dir, 
                            Device device)
{
    VkDevice        logicalDevice         = getLogicalDevice(device);
    void           *data                  = NULL;
    BBError         er                    = BB_ERROR_UNKNOWN;
    int             texWidth              = 0;
    int             texHeight             = 0; 
    int             texChannels           = 0;
    VkDeviceSize    imageSize             = {0};
    StagingBuffer   sBuffer               = NULL;
    VkDeviceMemory  stagingBufferMemory   = {0};
    // TODO: custom image loader?
    stbi_uc        *pixels                = stbi_load(dir, 
                                                      &texWidth, 
                                                      &texHeight, 
                                                      &texChannels, 
                                                      STBI_rgb_alpha);
    if(!pixels){
        return BB_ERROR_IMAGE_CREATE;
    }

    imageSize = texWidth * texHeight * 4;
    //TODO: MALLOC without free
    *image    = (VulkanImage_T*)calloc(1, sizeof(VulkanImage_T));
    if (image == NULL){
        return BB_ERROR_MEM;
    }
    (*image)->device = device;
    (*image)->format = VK_FORMAT_R8G8B8A8_SRGB;
    (*image)->width  = texWidth;
    (*image)->height = texHeight;
    (*image)->layerCount = 1; // TODO: placeholder + sensitive invariant

    createStagingBuffer   (&sBuffer, device, imageSize);
    copyIntoStagingBuffer (sBuffer, pixels, imageSize);
    stbi_image_free       (pixels);
    er = 
    createImage           (texWidth, texHeight, 
                           VK_FORMAT_R8G8B8A8_SRGB, 
                           VK_IMAGE_TILING_OPTIMAL, 
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                           (*image)->textureImage, 
                           (*image)->textureImageMemory, 
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
                    (*v)->textureImage, 
                    NULL);
    vkFreeMemory   (logicalDevice, 
                    (*v)->textureImageMemory, 
                    NULL);
    free           (*v);
    *v = NULL;
}

static BBError createImage(const uint32_t width, 
                           const uint32_t height, 
                           const VkFormat format, 
                           const VkImageTiling tiling, 
                           const VkImageUsageFlags usage, 
                           const VkMemoryPropertyFlags properties, 
                           VkImage image, 
                           VkDeviceMemory imageMemory, 
                           const Device device)
{
    VkMemoryAllocateInfo allocInfo       = {};
    VkMemoryRequirements memRequirements = {};
    VkImageCreateInfo    imageInfo       = {};
    VkDevice             logicalDevice   = getLogicalDevice(device);

    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags         = 0; // Optional
                                 //
    if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        return BB_ERROR_IMAGE_CREATE;
    }
    vkGetImageMemoryRequirements (logicalDevice, image, &memRequirements);

    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, device);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        return BB_ERROR_MEM;
    }

    vkBindImageMemory            (logicalDevice, image, imageMemory, 0);
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
    barrier.image                           = image->textureImage;
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
        return BB_ERROR_IMAGE_LAYOUT_TRANSISTION;
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

BBError createTextureImageView(VulkanImage image, ImageViewType type)
{
    VkDevice              logicalDevice = getLogicalDevice(image->device);
    VkImageViewCreateInfo viewInfo      = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image->textureImage;
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
                            image->textureImage,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            1,
                            &region);
    endSingleTimeCommands  (&commandBuffer, 
                            image->device);
}
