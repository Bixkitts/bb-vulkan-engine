#ifndef IMAGES
#define IMAGES
#include "device.hpp"

struct VulkanImage
{
    bve::Device* device;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
};
namespace bve
{
VulkanImage* createTextureImage(Device* device);
void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, Device* device);

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Device* device);

void destroyImage(VulkanImage *v);
}
#endif
