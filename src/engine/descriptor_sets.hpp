#ifndef BB_DESCRIPTOR_SETS
#define BB_DESCRIPTOR_SETS

#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "defines.hpp"
#include "pipeline.hpp"
#include "images.hpp"

typedef struct VulkanDescriptorPool
{
    VkDescriptorPool pool;
    Device *device;
}VulkanDescriptorPool;

enum BBDescriptorSetLayout
{
    // Values indicate size of the layouts
    DS_LAYOUT_BITCH_BASIC = 2,
    DS_LAYOUT_BASIC       = 2,
    DS_LAYOUT_WHATEVER    = 5
};

VkDescriptorSetLayout createDescriptorSetLayout(Device *device, BBDescriptorSetLayout layout);

VulkanDescriptorPool *createDescriptorPool(Device *device);

//TODO: Allow the linking of multiple textures instead of just one
std::vector<VkDescriptorSet> createDescriptorSets(Device *device, VkDescriptorSetLayout descriptorSetLayout, VulkanDescriptorPool *descriptorPool, std::vector<UniformBuffer*> &uniformBuffers, VulkanImage *texture);

#endif
