#include "descriptor_sets.hpp"
#include "buffers.hpp"
#include "device.hpp"
#include "images.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "config_pipeline.hpp"
#include "fileIO.hpp"

// Currently this function creates one predefined boring layout.
// Need to make this dynamic at some point, allowing
// lots of different resources to be bound.
// Perhaps I'll make a couple of predefined sets with
// specific combinations of descriptor slots.
VkDescriptorSetLayout createDescriptorSetLayout(Device *device, BBDescriptorSetLayout layoutSizeAndType)
{
    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorSetLayoutBinding bindings[layoutSizeAndType];
    int bindingCount = 0;

    // use the BBDescriptorSetLayout enum in this pattern
    // to build descriptor set layouts here
    if ((layoutSizeAndType == DS_LAYOUT_BITCH_BASIC) || (layoutSizeAndType == DS_LAYOUT_BASIC))
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding            = 0;
        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount    = 1;
        uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        bindings[bindingCount] = uboLayoutBinding;
        bindingCount++;
    }
    if ((layoutSizeAndType == DS_LAYOUT_BITCH_BASIC) || (layoutSizeAndType == DS_LAYOUT_BASIC))
    {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        bindings[bindingCount] = samplerLayoutBinding;
        bindingCount++;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutSizeAndType;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device->logical, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    return descriptorSetLayout;
}


VulkanDescriptorPool *createDescriptorPool(Device *device)
{
    auto descriptorPool = new VulkanDescriptorPool{};

    VkDescriptorPoolSize poolSizes[2] = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = (uint32_t)MAX_FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(device->logical, &poolInfo, nullptr, &descriptorPool->pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return descriptorPool;
}

VkDescriptorSet *createDescriptorSets(Device *device, 
                VkDescriptorSetLayout descriptorSetLayout, 
                VulkanDescriptorPool *descriptorPool, 
                UniformBuffer *uniformBuffers, 
                VulkanImage *texture)
{
    VkDescriptorSet *descriptorSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet)*MAX_FRAMES_IN_FLIGHT);

    // The layout of these sets need to all be the same,
    // as they will all be rendering the same resources.
    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        layouts[i] = descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool->pool;
    allocInfo.descriptorSetCount = (uint32_t)(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts;
    
    if (vkAllocateDescriptorSets(device->logical, &allocInfo, descriptorSets) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    const int AMOUNT_OF_DESCRIPTORS = 3;
    const int descriptorWriteCount = MAX_FRAMES_IN_FLIGHT * AMOUNT_OF_DESCRIPTORS;
    VkWriteDescriptorSet descriptorWrites[descriptorWriteCount] = {};
    for(int i = 0; i < descriptorWriteCount; i+=AMOUNT_OF_DESCRIPTORS)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //TODO: AHHHHH. Sort out the views and samplers thing.
        imageInfo.imageView   = texture->views[0];
        imageInfo.sampler     = texture->samplers[0];

        VkDescriptorBufferInfo transBufferInfo{};
        transBufferInfo.buffer = uniformBuffers[0].buffer;
        transBufferInfo.offset = 0;
        transBufferInfo.range  = sizeof(PerObjectMatrices);

        VkDescriptorBufferInfo viewBufferInfo{};
        viewBufferInfo.buffer  = uniformBuffers[1].buffer;
        viewBufferInfo.offset  = 0;
        viewBufferInfo.range   = sizeof(ViewMatrices);

        descriptorWrites[i].sType               = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet              = descriptorSets[i];
        descriptorWrites[i].dstBinding          = 0;
        descriptorWrites[i].dstArrayElement     = 0;
        descriptorWrites[i].descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[i].descriptorCount     = 1;
        descriptorWrites[i].pBufferInfo         = &transBufferInfo;

        descriptorWrites[i+1].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i+1].dstSet            = descriptorSets[i];
        descriptorWrites[i+1].dstBinding        = 1;
        descriptorWrites[i+1].dstArrayElement   = 0;
        descriptorWrites[i+1].descriptorType    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[i+1].descriptorCount   = 1;
        descriptorWrites[i+1].pImageInfo        = &imageInfo; // Optional
    }
    vkUpdateDescriptorSets(device->logical, descriptorWriteCount, descriptorWrites, 0, nullptr);
    return descriptorSets;
}

