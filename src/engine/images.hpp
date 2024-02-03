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
    // Texture image view types here
    TEXTURE_IMAGE_VIEW_DEFAULT   = 0,
    // Depth image view types here
    DEPTH_IMAGE_VIEW_DEFAULT     = 0,
    // Swapchain image view types here
    SWAPCHAIN_IMAGE_VIEW_DEFAULT = 0
}ImageViewType;


// This should (at minimum) correspond to the size of the biggest
// enum above.
// It's the size of the array that holds views into an image,
// and the enums index directly into it.
#define IMAGE_VIEW_TYPES_MAX 4

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
                                       ImageViewType type);
BBError     createTextureSampler      (VulkanImage image, 
                                       ImageSamplerType type);
/* Depth images (typically for the swapchain) */
BBError     createDepthImage          (VulkanImage *image, 
                                       VkExtent2D extent);
BBError     createDepthImageView      (VulkanImage image, 
                                       ImageViewType type);
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
                                       ImageViewType viewType);
VkSampler   getImageSampler           (VulkanImage image,
                                       ImageSamplerType samplerType);
VkFormat    getImageFormat            (VulkanImage image);
BBError     transitionImageLayout     (VulkanImage image, 
                                       VkImageLayout oldLayout, 
                                       VkImageLayout newLayout);
void        destroyImage              (VulkanImage *v);

#endif
