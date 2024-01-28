#ifndef BB_IMAGES
#define BB_IMAGES

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "defines.hpp"
#include "device.hpp"
#include "error_handling.h"
#include "defines.hpp"

typedef enum 
{
    TEXTURE_IMAGE_VIEW_DEFAULT,
    TEXTURE_IMAGE_VIEW_COUNT
}TextureImageViewType;

typedef enum
{
    DEPTH_IMAGE_VIEW_DEFAULT,
    DEPTH_IMAGE_VIEW_COUN
}DepthImageViewType;

typedef enum
{
    IMAGE_SAMPLER_DEFAULT,
    IMAGE_SAMPLER_COUNT
}ImageSamplerType;

typedef struct VulkanImage_T VulkanImage_T;
BB_OPAQUE_HANDLE(VulkanImage);

/* Texture images (typically for fragment shaders) */
BBError     createTextureImage        (VulkanImage *image, 
                                       const char *dir, 
                                       const Device device);
BBError     createTextureImageView    (VulkanImage image, 
                                       TextureImageViewType type);
BBError     createTextureSampler      (VulkanImage image, 
                                       ImageSamplerType type);
/* Depth images (typically for the swapchain) */
BBError     createDepthImage          (VulkanImage *image, 
                                       VkExtent2D extent);
BBError     createDepthImageView      (VulkanImage image, 
                                       DepthImageViewType type);
/* Swapchain image stuff. This is pretty messy! */
BBError     createSwapchainImages     (VulkanImage images[], 
                                       Device device, 
                                       VkSwapchainKHR swapchain,
                                       uint32_t *count);
BBError     createSwapchainImageViews (VulkanImage swapchainImages[], 
                                       uint32_t imageCount);

/* Any image type */
VkImage     getImageHandle            (VulkanImage image);
VkImageView getImageView              (VulkanImage image,
                                       TextureImageViewType viewType);
VkSampler   getImageSampler           (VulkanImage image,
                                       ImageSamplerType samplerType);
BBError     transitionImageLayout     (VulkanImage image, 
                                       VkImageLayout oldLayout, 
                                       VkImageLayout newLayout);
void        destroyImage              (VulkanImage *v);

#endif
