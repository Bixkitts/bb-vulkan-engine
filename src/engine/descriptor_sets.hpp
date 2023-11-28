#ifndef BB_DESCRIPTOR_SETS
#define BB_DESCRIPTOR_SETS

#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "defines.hpp"
#include "pipeline.hpp"
#include "images.hpp"

// TODO: do I really need to have these in a struct?
typedef struct VulkanDescriptorPool
{
    VkDescriptorPool pool;
    Device device;
}VulkanDescriptorPool;

enum BBDescriptorSetLayout
{
    DS_LAYOUT_BITCH_BASIC,
    DS_LAYOUT_BASIC,
    DS_LAYOUT_WHATEVER,
    DS_LAYOUT_AMOUNT_OF_LAYOUTS /* Keep this at the end of the enum */
};

BBError createDescriptorSetLayout(VkDescriptorSetLayout layout, Device device, BBDescriptorSetLayout layoutSizeAndType);

BBError createDescriptorPool(VulkanDescriptorPool *pool, Device device);

//TODO: Allow the linking of multiple textures instead of just one
BBError createDescriptorSets(VkDescriptorSet *descriptorSets, Device device, VkDescriptorSetLayout descriptorSetLayout, VulkanDescriptorPool *descriptorPool, UniformBuffer *uniformBuffers, VulkanImage *texture);

#endif
