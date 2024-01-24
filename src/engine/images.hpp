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
    IMAGE_VIEW_DEFAULT,
    IMAGE_VIEW_COUNT
}ImageViewType;

typedef enum
{
    IMAGE_SAMPLER_DEFAULT,
    IMAGE_SAMPLER_COUNT
}ImageSamplerType;

typedef struct VulkanImage_T VulkanImage_T;
BB_OPAQUE_HANDLE(VulkanImage);

BBError     createTextureImage     (VulkanImage *image, 
                                    const char *dir, 
                                    const Device device);
BBError     createTextureImageView (VulkanImage image, ImageViewType type);
BBError     createTextureSampler   (VulkanImage image, ImageSamplerType type);
BBError     transitionImageLayout  (VulkanImage_T *image, 
                                    VkImageLayout oldLayout, 
                                    VkImageLayout newLayout);
VkImageView getTextureImageView    (VulkanImage image,
                                    ImageViewType viewType);
VkSampler   getTextureImageSampler (VulkanImage image,
                                    ImageSamplerType samplerType);
void        destroyImage           (VulkanImage *v);

#endif
