#ifndef IMAGES
#define IMAGES
#include "device.hpp"

struct VulkanImage
{
    bve::Device* device;
    VkFormat format;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
};

struct VulkanImageView
{
    bve::Device* device;
    VkImageView view;
};

struct VulkanSampler
{
    bve::Device* device;
    VkSampler sampler;
};

namespace bve
{
VulkanImage* createTextureImage(Device* device);
VulkanImageView* createTextureImageView(VulkanImage* image);
VulkanSampler* createTextureSampler(VulkanImage* image);
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Device* device);

void destroyImage(VulkanImage *v);
void destroyImageView(VulkanImageView *v);
void destroySampler(VulkanSampler *s);

}
#endif
