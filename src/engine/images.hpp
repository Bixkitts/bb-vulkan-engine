#ifndef IMAGES
#define IMAGES

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <stb_image.h>

#include "defines.hpp"
#include "device.hpp"
#include "error_handling.h"
#include "buffers.hpp"
#include "command_buffers.hpp"
#include "defines.hpp"

struct VulkanImage
{
    Device* device;
    VkFormat format;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    // Views of the image, there will typically be only one
    // anyways.
    // If there are more of them I'll need
    // to come up with a standard for how they are
    // dereferenced.
    VkImageView views[VIEW_PER_IMAGE];
    uint64_t viewCount;
    // I'll also need some kind of convention
    // for referencing the correct samplers
    // when there are multiple.
    // A problem for future me!
    VkSampler samplers[SAMPLER_PER_IMAGE];
    uint64_t samplerCount;
};

BBError createTextureImage(VulkanImage *image, char *dir, Device *device);
void createTextureImageView(VulkanImage* image);
void createTextureSampler(VulkanImage* image);
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Device* device);

void destroyImage(VulkanImage *v);

#endif
