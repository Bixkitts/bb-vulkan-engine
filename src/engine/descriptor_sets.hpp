#ifndef BB_DESCRIPTOR_SETS
#define BB_DESCRIPTOR_SETS

#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "buffers.hpp"
#include "images.hpp"

// TODO: do I really need to have these in a struct?
typedef struct VulkanDescriptorPool_T
{
    VkDescriptorPool pool;
    Device           device;
}VulkanDescriptorPool_T, *VulkanDescriptorPool;

typedef VkDescriptorSet *VkDescriptorSetArray;

enum BBDescriptorSetLayout
{
    DS_LAYOUT_BITCH_BASIC,
    DS_LAYOUT_BASIC,
    DS_LAYOUT_WHATEVER,
    DS_LAYOUT_COUNT
};

BBError createDescriptorSetLayout  (VkDescriptorSetLayout *layout, 
                                    const Device device, 
                                    const BBDescriptorSetLayout layoutSizeAndType);

BBError createDescriptorPool       (VulkanDescriptorPool *pool, 
                                    const Device device);

//TODO: Allow the binding of multiple textures instead of just one
BBError createDescriptorSets       (VkDescriptorSetArray *descriptorSets, 
                                    const int dSetCount,
                                    const Device device, 
                                    const VkDescriptorSetLayout descriptorSetLayout, 
                                    const VulkanDescriptorPool descriptorPool, 
                                    const UniformBuffer uniformBuffers[], 
                                    const VulkanImage texture);

#endif
