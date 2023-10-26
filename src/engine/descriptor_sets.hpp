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

//TODO: Allow the linking of multiple textures instead of just one
std::vector<VkDescriptorSet> createDescriptorSets(Device *device, VkDescriptorSetLayout descriptorSetLayout, VulkanDescriptorPool *descriptorPool, std::vector<UniformBuffer*> &uniformBuffers, VulkanImage *texture);

}

#endif
