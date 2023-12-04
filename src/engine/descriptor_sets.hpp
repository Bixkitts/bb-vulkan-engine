#ifndef BB_DESCRIPTOR_SETS
#define BB_DESCRIPTOR_SETS

#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "buffers.hpp"
#include "images.hpp"
#include "defines.hpp"
#include "swap_chain.hpp"

// TODO: do I really need to have these in a struct?
typedef struct VulkanDescriptorPool_S
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
    DS_LAYOUT_AMOUNT_OF_LAYOUTS /* Keep this at the end of the enum */
};

BBError createDescriptorSetLayout  (VkDescriptorSetLayout *layout, 
                                    const Device device, 
                                    const BBDescriptorSetLayout layoutSizeAndType);

BBError createDescriptorPool       (VulkanDescriptorPool pool, 
                                    const Device device);

//TODO: Allow the linking of multiple textures instead of just one
BBError createDescriptorSets       (VkDescriptorSetArray *descriptorSets, 
                                    const Device device, 
                                    const VkDescriptorSetLayout descriptorSetLayout, 
                                    const VulkanDescriptorPool *descriptorPool, 
                                    const UniformBufferArray uniformBuffers, 
                                    const VulkanImage texture);

#endif
