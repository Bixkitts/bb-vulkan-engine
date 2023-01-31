#include "pipeline.hpp"
#include "buffers.hpp"
#include "device.hpp"
#include "model.hpp"
#include "swap_chain.hpp"
#include "config_pipeline.hpp"
#include "fileIO.hpp"

//std libraries
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vulkan/vulkan_core.h>
#include <cassert>
#include <stdexcept>

namespace bve
{

GraphicsPipeline* createGraphicsPipeline(
        Device* device,
        SwapChain* swapchain,
        const std::string& vertFilepath, 
        const std::string& fragFilepath,
        PipelineConfig* configInfo) // Here a default configuration is given in atm
{
    auto mainPipeline = new GraphicsPipeline{};


    assert(configInfo->pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
    assert(configInfo->renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");
    const std::vector<char> vertCode = readFile(vertFilepath);
    const std::vector<char> fragCode = readFile(fragFilepath);
    std::cout<< "Vertex Shader Code Size: " << vertCode.size() << '\n';
    std::cout<< "Fragment Shader Code Size: " << fragCode.size() << '\n';


    mainPipeline->device = device;
    mainPipeline->swapchain = swapchain;
    createVertShaderModule(mainPipeline, vertCode);             //These create the VkShaderModules
    createFragShaderModule(mainPipeline, fragCode);             //stored in the GraphicsPipeline

    
    mainPipeline->pipelineConfig = std::move(configInfo);       //The default configuration I've passed in
                                                                //stays stored in the pipeline object 
                                                                //for reference and vulkan deallocation

    //Here the vertex input info is put constructed 
    auto bindingDescriptions     = getBindingDescriptions();
    auto attributeDescriptions   = getAttributeDescriptions();
    auto *vertexInputInfo        = config::vertexInputStateCreateInfo(&bindingDescriptions, &attributeDescriptions);
    //Some more configuration
    auto *shaderStages           = config::shaderStagesCreateInfo(mainPipeline);
    auto *viewportInfo           = config::viewportCreateInfo(mainPipeline->pipelineConfig);

    //Uses info from all the above objects and is translated to a final configuration
    //to the actual VkPipeline.
    auto *pipelineCreateInfo     = config::pipelineCreateInfo(configInfo,
                                                              viewportInfo,
                                                              shaderStages,
                                                              vertexInputInfo);
    //-------------------------------------------------------------------


    if(vkCreateGraphicsPipelines(device->logical,
                                 VK_NULL_HANDLE,
                                 1,
                                 pipelineCreateInfo,
                                 nullptr,
                                 &mainPipeline->graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("pipeline not created successfully \n");
    }
    cleanupShaderModules(mainPipeline);
    return mainPipeline;
}

void destroyPipeline(GraphicsPipeline* pipeline)
{
    vkDestroyPipelineLayout(pipeline->device->logical, pipeline->pipelineConfig->pipelineLayout, nullptr);
    vkDestroyDescriptorPool(pipeline->device->logical, pipeline->pipelineConfig->descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(pipeline->device->logical, pipeline->pipelineConfig->descriptorSetLayout, nullptr);
    vkDestroyPipeline(pipeline->device->logical, pipeline->graphicsPipeline, nullptr);
    delete pipeline->pipelineConfig;
    delete pipeline;
}
 
static void cleanupShaderModules(GraphicsPipeline* pipeline)
{
    vkDestroyShaderModule(pipeline->device->logical, pipeline->vertShaderModule, nullptr);
    vkDestroyShaderModule(pipeline->device->logical, pipeline->fragShaderModule, nullptr);
}

PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain, std::vector<UniformBuffer*> &uniformBuffers)
{
    auto *config = config::pipelineConfigDefault(swapchain, uniformBuffers);
    return config;
}

static void createVertShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code)
{
    pipeline->vertShaderModule = {};
    auto createInfo = config::shaderModuleInfo(code);


    if (vkCreateShaderModule(pipeline->device->logical, createInfo, nullptr, &pipeline->vertShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module \n");        
    }
    delete createInfo;
}

static void createFragShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code)
{
    pipeline->fragShaderModule = {};
    auto createInfo = config::shaderModuleInfo(code);


    if (vkCreateShaderModule(pipeline->device->logical, createInfo, nullptr, &pipeline->fragShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module \n");        
    }
    delete createInfo;
}

VkDescriptorPool createDescriptorPool(Device *device)
{

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPool descriptorPool;

    if (vkCreateDescriptorPool(device->logical, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return descriptorPool;
}

std::vector<VkDescriptorSet> createDescriptorSets(Device *device, PipelineConfig *config)
{
    std::vector<VkDescriptorSet> descriptorSets{};

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, config->descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = config->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();
    
    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device->logical, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = config->uniformBuffers[i]->buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(Matrices);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional
                                                    //
        vkUpdateDescriptorSets(device->logical, 1, &descriptorWrite, 0, nullptr);
    }

    return descriptorSets;

}

VkPipelineLayout createPipelineLayout(Device *device, VkDescriptorSetLayout *descriptorSetLayout)
{
    VkPipelineLayout pipelineLayout = {};
    auto pipelineLayoutInfo = config::pipelineLayoutCreateInfo(descriptorSetLayout);

    if(vkCreatePipelineLayout(device->logical, pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    delete pipelineLayoutInfo;
    return pipelineLayout;
}

void bindPipeline(GraphicsPipeline* pipeline, VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
}

VkDescriptorSetLayout createDescriptorSetLayout(Device *device)
{
    VkDescriptorSetLayout descriptorSetLayout = {};

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(device->logical, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return descriptorSetLayout;

}
}
