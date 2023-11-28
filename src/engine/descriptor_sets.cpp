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
BBError createDescriptorSetLayout(VkDescriptorSetLayout layout, 
                                  Device *device, 
                                  BBDescriptorSetLayout layoutSizeAndType)
{
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
    layoutInfo.bindingCount = bindingCount;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device->logical, &layoutInfo, nullptr, &layout)
            != VK_SUCCESS) 
    {
        return BB_ERROR_DESCRIPTOR_LAYOUT;
    }
    return BB_ERROR_OK;
}


BBError createDescriptorPool(VulkanDescriptorPool *pool, Device *device)
{
    // TODO: MALLOC without free
    pool = (VulkanDescriptorPool*)calloc(1, sizeof(VulkanDescriptorPool));
    if (pool == NULL) {
        return BB_ERROR_MEM;
    }
    // TODO: magic number 2?
    VkDescriptorPoolSize poolSizes[2] = {};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount       = 2;
    poolInfo.pPoolSizes          = poolSizes;
    poolInfo.maxSets             = (uint32_t)MAX_FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(device->logical, &poolInfo, nullptr, &pool->pool) != VK_SUCCESS) {
        fprintf(stderr, "\nfailed to create descriptor pool");
        return BB_ERROR_DESCRIPTOR_POOL;
    }
    return BB_ERROR_OK;
}

BBError createDescriptorSets(VkDescriptorSet *descriptorSets,
                             Device *device, 
                             VkDescriptorSetLayout descriptorSetLayout, 
                             VulkanDescriptorPool *descriptorPool, 
                             UniformBuffer *uniformBuffers, 
                             VulkanImage *texture)
{
    // TODO: MALLOC without free
    descriptorSets = (VkDescriptorSet*)calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));
    if (descriptorSets == NULL) {
        return BB_ERROR_MEM;
    }

    // The layout of these sets need to all be the same,
    // as they will all be rendering the same resources.
    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        layouts[i] = descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool->pool;
    allocInfo.descriptorSetCount = (uint32_t)(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts        = layouts;
    
    if (vkAllocateDescriptorSets(device->logical, &allocInfo, descriptorSets) != VK_SUCCESS) {
        return BB_ERROR_DESCRIPTOR_SET;
    }
    // TODO: magic number 3
    const int AMOUNT_OF_DESCRIPTORS = 3;
    const int descriptorWriteCount = MAX_FRAMES_IN_FLIGHT * AMOUNT_OF_DESCRIPTORS;
    VkWriteDescriptorSet descriptorWrites[descriptorWriteCount];
    for(int i = 0; i < descriptorWriteCount; i+=AMOUNT_OF_DESCRIPTORS)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //TODO: AHHHHH. Sort out the views and samplers thing.
        //They need to be encoded depending on what image or sampler type it is
        imageInfo.imageView   = texture->views[0];
        imageInfo.sampler     = texture->samplers[0];

        // TODO: moar uniform buffers is possible....
        VkDescriptorBufferInfo transBufferInfo{};
        transBufferInfo.buffer = uniformBuffers[0].buffer;
        transBufferInfo.offset = 0;
        transBufferInfo.range  = sizeof(PerObjectMatrices);

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
    return BB_ERROR_OK;
}

