//std libraries
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <stdexcept>

#include <vulkan/vulkan_core.h>

#include "pipeline.hpp"
#include "buffers.hpp"
#include "device.hpp"
#include "model.hpp"
#include "swap_chain.hpp"
#include "config_pipeline.hpp"
#include "fileIO.hpp"

//Make shader Modules
static void createVertShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
static void createFragShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
static void cleanupShaderModules(GraphicsPipeline* pipeline);

GraphicsPipeline* createGraphicsPipeline(
        Device* device,
        SwapChain* swapchain,
        const std::string& vertFilepath, 
        const std::string& fragFilepath,
        PipelineConfig* configInfo)
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
    auto *vertexInputInfo        = vertexInputStateCreateInfo(&bindingDescriptions, &attributeDescriptions);
    //Some more configuration
    auto *shaderStages           = shaderStagesCreateInfo(mainPipeline);
    auto *viewportInfo           = viewportCreateInfo(mainPipeline->pipelineConfig);

    //Uses info from all the above objects and is translated to a final configuration
    //to the actual VkPipeline.
    auto *pipelineCreateInfo     = createPipelineCreateInfo(configInfo,
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
    //decouple cleanup from pipeline maybe
    
    vkDestroyPipelineLayout(pipeline->device->logical, pipeline->pipelineConfig->pipelineLayout, nullptr);
    //vkDestroyDescriptorPool(pipeline->device->logical, pipeline->pipelineConfig->descriptorPool, nullptr);
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

PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain, std::vector<UniformBuffer*> &uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, std::vector<VkDescriptorSet> &descriptorSets)
{
    auto *config = pipelineConfigDefault(swapchain, uniformBuffers, descriptorSetLayout, descriptorSets);
    return config;
}

static void createVertShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code)
{
    pipeline->vertShaderModule = {};
    auto createInfo = shaderModuleInfo(code);


    if (vkCreateShaderModule(pipeline->device->logical, &createInfo, nullptr, &pipeline->vertShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module \n");        
    }
}

static void createFragShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code)
{
    pipeline->fragShaderModule = {};
    auto createInfo = shaderModuleInfo(code);


    if (vkCreateShaderModule(pipeline->device->logical, &createInfo, nullptr, &pipeline->fragShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module \n");        
    }
}

VkPipelineLayout createPipelineLayout(Device *device, PipelineConfig *config)
{
    VkPipelineLayout pipelineLayout = {};
    auto pipelineLayoutInfo = pipelineLayoutCreateInfo(config);

    if(vkCreatePipelineLayout(device->logical, &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    return pipelineLayout;
}

void bindPipeline(GraphicsPipeline* pipeline, VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
}
