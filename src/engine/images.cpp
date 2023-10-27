#include <stb_image.h>

#include "images.hpp"
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "buffers.hpp"
#include "command_buffers.hpp"
#include "defines.hpp"

#define STB_IMAGE_IMPLEMENTATION

static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, Device* device);

// Load a texture into a buffer from
// a directory on disk.
// Currently allocates on load and delete, I need
// to get my memory management together!
VulkanImage* createTextureImage(char* dir, Device* device)
{
    int texWidth, texHeight, texChannels;
    // Perhaps one day I should use a custom image loader....
    // *uuuuurgh*
    stbi_uc* pixels = stbi_load(dir, 
            &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if(!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            stagingBuffer, 
            stagingBufferMemory, device);
    void* data;
    vkMapMemory(device->logical, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device->logical, stagingBufferMemory);
    stbi_image_free(pixels);


    auto image = new VulkanImage{};
    image->device = device;
    image->format = VK_FORMAT_R8G8B8A8_SRGB;

    createImage(texWidth, texHeight, 
            VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            image->textureImage, image->textureImageMemory, device);
    transitionImageLayout(image->textureImage, 
            VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, device);
    copyBufferToImage(device, stagingBuffer, 
            image->textureImage, 
            static_cast<uint32_t>(texWidth), 
            static_cast<uint32_t>(texHeight), 1 );
    transitionImageLayout(image->textureImage, 
            VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, device);

    
    vkDestroyBuffer(device->logical, stagingBuffer, nullptr);
    // yuck, reuse allocated memory!
    vkFreeMemory(device->logical, stagingBufferMemory, nullptr);
    
    return image;
}

void destroyImage(VulkanImage *v)
{
    for(uint64_t i = v->viewCount-1; i >= 0; i--)
    {
        vkDestroyImageView(v->device->logical, v->views[i], nullptr);
    }
    for(uint64_t i = v->samplerCount-1; i >= 0; i--)
    {
        vkDestroySampler(v->device->logical, v->samplers[i], nullptr);
    }
    vkDestroyImage(v->device->logical, v->textureImage, nullptr);
    vkFreeMemory(v->device->logical, v->textureImageMemory, nullptr);
    delete v;
}

static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, Device* device)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional
    //-------------------------------
    if (vkCreateImage(device->logical, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->logical, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, device);

    if (vkAllocateMemory(device->logical, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device->logical, image, imageMemory, 0);
}

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Device* device) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } 
    else 
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

     vkCmdPipelineBarrier(                      // vkCmdPipelineBarrier(
        commandBuffer,
        0 /* TODO */, 0 /* TODO */,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer, device);
}

// this has a duplicate function I should transform into one
// ^ I read this later and have no idea what the duplicate is
// This function is a bitch basic placeholder
// that simply reads a 2D color texture.
// I should maybe generalise it more.
void createTextureImageView(VulkanImage* image)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image->textureImage;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = image->format;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (image->viewCount <= VIEW_PER_IMAGE)
    {
        if (vkCreateImageView(image->device->logical, &viewInfo, nullptr, &image->views[image->viewCount]) !=
                VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture image view!");
        }
            image->viewCount++;
    }
    else
    {
        throw std::runtime_error("too many image views for one image!");
    }
}

void createTextureSampler(VulkanImage* image)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType                    = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter                = VK_FILTER_LINEAR;
    samplerInfo.minFilter                = VK_FILTER_LINEAR;
    samplerInfo.addressModeU             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable         = VK_TRUE;
    samplerInfo.maxAnisotropy            = image->device->physicalProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor              = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates  = VK_FALSE;
    samplerInfo.compareEnable            = VK_FALSE;
    samplerInfo.compareOp                = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode               = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias               = 0.0f;
    samplerInfo.minLod                   = 0.0f;
    samplerInfo.maxLod                   = 0.0f;


    if (image->samplerCount <= SAMPLER_PER_IMAGE)
    {
        if (vkCreateSampler(image->device->logical, 
                    &samplerInfo, 
                    nullptr, 
                    &image->samplers[image->samplerCount]) 
                    != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
            image->samplerCount++;
    }
    else
    {
        throw std::runtime_error("too many samplers for one image!");
    }
}
