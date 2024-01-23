#ifndef BB_IMAGES
#define BB_IMAGES

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "defines.hpp"
#include "device.hpp"
#include "error_handling.h"
#include "defines.hpp"

typedef struct VulkanImage_T VulkanImage_T;
BB_OPAQUE_HANDLE(VulkanImage);

BBError createTextureImage     (VulkanImage *image, 
                                const char *dir, 
                                const Device device);
BBError createTextureImageView (VulkanImage image);
BBError createTextureSampler   (VulkanImage image);
BBError transitionImageLayout  (VkImage image, 
                                const VkFormat format,  
                                VkImageLayout oldLayout, 
                                VkImageLayout newLayout, 
                                const Device device);
void    destroyImage           (VulkanImage *v);

#endif
