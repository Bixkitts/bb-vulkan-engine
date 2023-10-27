#ifndef DESCRIPTOR_SETS
#define DESCRIPTOR_SETS

#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "pipeline.hpp"
#include "images.hpp"

struct VulkanDescriptorPool
{
    VkDescriptorPool pool;
    Device *device;
};

enum BBDescriptorSetLayout
{
    // Values indicate size of the layouts
    BITCH_BASIC = 2,
    BASIC       = 2,
    WHATEVER    = 5
};

VkDescriptorSetLayout createDescriptorSetLayout(Device *device, BBDescriptorSetLayout layout);

VulkanDescriptorPool *createDescriptorPool(Device *device);

//TODO: Allow the linking of multiple textures instead of just one
std::vector<VkDescriptorSet> createDescriptorSets(Device *device, VkDescriptorSetLayout descriptorSetLayout, VulkanDescriptorPool *descriptorPool, std::vector<UniformBuffer*> &uniformBuffers, VulkanImage *texture);

#endif
