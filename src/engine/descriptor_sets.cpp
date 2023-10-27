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
    if ((layoutSizeAndType == BITCH_BASIC) || (layoutSizeAndType == BASIC))
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
    if ((layoutSizeAndType == BITCH_BASIC) || (layoutSizeAndType == BASIC))
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
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device->logical, &poolInfo, nullptr, &descriptorPool->pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return descriptorPool;
}

std::vector<VkDescriptorSet> createDescriptorSets(Device *device, 
                VkDescriptorSetLayout descriptorSetLayout, 
                VulkanDescriptorPool *descriptorPool, 
                std::vector<UniformBuffer*> &uniformBuffers, 
                VulkanImage *texture)
{
    std::vector<VkDescriptorSet> descriptorSets{};

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool->pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();
    
    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device->logical, &allocInfo, descriptorSets.data()) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i]->buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(PerObjectMatrices);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //TODO: AHHHHH. Sort out the views and samplers thing.
        imageInfo.imageView = texture->views[0];
        imageInfo.sampler = texture->samplers[0];

        VkWriteDescriptorSet descriptorWrites[2] = {};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo; // Optional
                                                    //
        vkUpdateDescriptorSets(device->logical, 2, descriptorWrites, 0, nullptr);
    }

    return descriptorSets;

}

