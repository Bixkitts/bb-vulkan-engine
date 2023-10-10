#ifndef DESCRIPTOR_SETS
#define DESCRIPTOR_SETS
#include "device.hpp"
#include "pipeline.hpp"
#include "images.hpp"
#include <vulkan/vulkan_core.h>
namespace bve
{
struct VulkanDescriptorPool
{
    VkDescriptorPool pool;
    Device *device;
};

VkDescriptorSetLayout createDescriptorSetLayout(Device *device);

VulkanDescriptorPool *createDescriptorPool(Device *device);

std::vector<VkDescriptorSet> createDescriptorSets(Device *device, VkDescriptorSetLayout descriptorSetLayout, VulkanDescriptorPool *descriptorPool, std::vector<UniformBuffer*> &uniformBuffers, VulkanImageView *textureImageView, VulkanSampler *textureSampler);

}

#endif
