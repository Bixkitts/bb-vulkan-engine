#include "descriptor_sets.hpp"

// Currently this function creates one predefined boring layout.
// Need to make this dynamic at some point, allowing
// lots of different resources to be bound.
// Perhaps I'll make a couple of predefined sets with
// specific combinations of descriptor slots.
BBError createDescriptorSetLayout(VkDescriptorSetLayout *layout, 
                                  const Device device, 
                                  const BBDescriptorSetLayout layoutType)
{
    VkDescriptorSetLayoutBinding    bindings[MAX_BINDINGS_PER_LAYOUT];
    VkDescriptorSetLayoutBinding    uboLayoutBinding     = {};
    VkDescriptorSetLayoutBinding    samplerLayoutBinding = {};
    VkDescriptorSetLayoutCreateInfo layoutInfo           = {};
    int                             bindingCount         = 0;

    // use the BBDescriptorSetLayout enum in this pattern
    // to build descriptor set layouts here
    if ((layoutType == DS_LAYOUT_BITCH_BASIC) 
    || (layoutType == DS_LAYOUT_BASIC)){
        uboLayoutBinding.binding                = 0;
        uboLayoutBinding.descriptorType         = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount        = 1;
        uboLayoutBinding.stageFlags             = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers     = NULL; // Optional

        bindings[bindingCount] = uboLayoutBinding;
        bindingCount++;
    }
    if ((layoutType == DS_LAYOUT_BITCH_BASIC) 
    || (layoutType == DS_LAYOUT_BASIC))
    {
        samplerLayoutBinding.binding            = 1;
        samplerLayoutBinding.descriptorCount    = 1;
        samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = NULL;
        samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

        bindings[bindingCount] = samplerLayoutBinding;
        bindingCount++;
    }

    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindingCount;
    layoutInfo.pBindings    = bindings;

    if (vkCreateDescriptorSetLayout(device->logical, 
                                    &layoutInfo, 
                                    NULL, 
                                    layout)
    != VK_SUCCESS){
        return BB_ERROR_DESCRIPTOR_LAYOUT;
    }
    return BB_ERROR_OK;
}

BBError createDescriptorPool(VulkanDescriptorPool pool, 
                             const Device device)
{
    // TODO: magic number 2
    VkDescriptorPoolSize       poolSizes[2] = {};
    VkDescriptorPoolCreateInfo poolInfo     = {};

    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

    poolInfo.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount       = 2;
    poolInfo.pPoolSizes          = poolSizes;
    poolInfo.maxSets             = (uint32_t)MAX_FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(device->logical, 
                               &poolInfo, 
                               NULL, 
                               &pool->pool) 
    != VK_SUCCESS){
        fprintf(stderr, "\nfailed to create descriptor pool");
        return BB_ERROR_DESCRIPTOR_POOL;
    }
    return BB_ERROR_OK;
}

BBError createDescriptorSets(VkDescriptorSetArray *descriptorSets,
                             const Device device, 
                             const VkDescriptorSetLayout descriptorSetLayout, 
                             const VulkanDescriptorPool descriptorPool, 
                             const UniformBufferArray uniformBuffers, 
                             const VulkanImage texture)
{
    const int                   AMOUNT_OF_DESCRIPTORS                  = 3;
    const int                   descriptorWriteCount                   = 
                                MAX_FRAMES_IN_FLIGHT * AMOUNT_OF_DESCRIPTORS;
    VkDescriptorSetLayout       layouts[MAX_FRAMES_IN_FLIGHT]          = {0};
    VkDescriptorSetAllocateInfo allocInfo                              = {};
    VkWriteDescriptorSet        descriptorWrites[descriptorWriteCount] = {};
    VkDescriptorImageInfo       imageInfo                              = {};
    // TODO: MALLOC without free
    *descriptorSets = (VkDescriptorSet*)calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));
    if ((*descriptorSets) == NULL) {
        return BB_ERROR_MEM;
    }
    // The layout of these sets need to all be the same,
    // as they will all be rendering the same resources.
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        layouts[i] = descriptorSetLayout;
    }

    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool->pool;
    allocInfo.descriptorSetCount = (uint32_t)(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts        = layouts;
    
    if (vkAllocateDescriptorSets(device->logical, 
                                 &allocInfo, 
                                 *descriptorSets) 
        != VK_SUCCESS){
        return BB_ERROR_DESCRIPTOR_SET;
    }
    // TODO: magic number 3
    for(int i = 0; i < descriptorWriteCount; i += AMOUNT_OF_DESCRIPTORS){
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //TODO: AHHHHH. Sort out the views and samplers thing.
        //They need to be encoded depending on what image or sampler type it is
        imageInfo.imageView   = texture->views[0];
        imageInfo.sampler     = texture->samplers[0];

        VkDescriptorBufferInfo transBufferInfo{};
        transBufferInfo.buffer = uniformBuffers[0]->buffer;
        transBufferInfo.offset = 0;
        transBufferInfo.range  = sizeof(PerObjectMatrices);

        descriptorWrites[i].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet            = (*descriptorSets)[i];
        descriptorWrites[i].dstBinding        = 0;
        descriptorWrites[i].dstArrayElement   = 0;
        descriptorWrites[i].descriptorType    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[i].descriptorCount   = 1;
        descriptorWrites[i].pBufferInfo       = &transBufferInfo;

        descriptorWrites[i+1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i+1].dstSet          = (*descriptorSets)[i];
        descriptorWrites[i+1].dstBinding      = 1;
        descriptorWrites[i+1].dstArrayElement = 0;
        descriptorWrites[i+1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[i+1].descriptorCount = 1;
        descriptorWrites[i+1].pImageInfo      = &imageInfo; // Optional
    }
    vkUpdateDescriptorSets(device->logical, 
                           descriptorWriteCount, 
                           descriptorWrites, 
                           0, 
                           NULL);
    return BB_ERROR_OK;
}
