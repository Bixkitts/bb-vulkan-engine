#include "bve_pipeline.hpp"
#include "bve_device.hpp"
#include "bve_model.hpp"
#include "bve_swap_chain.hpp"
#include "config_pipeline.hpp"
#include "bve_fileIO.hpp"

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
        const std::string& vertFilepath, 
        const std::string& fragFilepath,
        PipelineConfig* configInfo) // Here a default configuration is given in atm
{
    assert(configInfo->pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
    assert(configInfo->renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");
    const std::vector<char> vertCode = readFile(vertFilepath);
    const std::vector<char> fragCode = readFile(fragFilepath);
    std::cout<< "Vertex Shader Code Size: " << vertCode.size() << '\n';
    std::cout<< "Fragment Shader Code Size: " << fragCode.size() << '\n';

    auto mainPipeline = new GraphicsPipeline{};

    mainPipeline->device = device;
    createVertShaderModule(mainPipeline, vertCode); //These create the VkShaderModules
    createFragShaderModule(mainPipeline, fragCode); //stored in the GraphicsPipeline

    mainPipeline->pipelineConfig = configInfo;      //The default configuration I've passed in
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
    auto *pipelineCreateInfo     = config::pipelineCreateInfo(mainPipeline->pipelineConfig,
                                                              viewportInfo,
                                                              shaderStages,
                                                              vertexInputInfo);
    //-------------------------------------------------------------------
    //These are two VkObjets that will need to have their destructors called.
    createPipelineLayout(mainPipeline);
    createRenderPass(mainPipeline);

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
    vkDestroyRenderPass(pipeline->device->logical, VkRenderPass , nullptr);
    vkDestroyPipeline(pipeline->device->logical, pipeline->graphicsPipeline, nullptr);
    delete pipeline->pipelineConfig;
    delete pipeline;
}
 
static void cleanupShaderModules(GraphicsPipeline* pipeline)
{
    vkDestroyShaderModule(pipeline->device->logical, pipeline->vertShaderModule, nullptr);
    vkDestroyShaderModule(pipeline->device->logical, pipeline->fragShaderModule, nullptr);
}

PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain)
{
    auto *config = config::pipelineConfigDefault(swapchain);
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

static void createPipelineLayout(GraphicsPipeline *pipeline)
{
    pipeline->pipelineConfig->pipelineLayout = {};
    auto pipelineLayoutInfo = config::pipelineLayoutCreateInfo();

    if(vkCreatePipelineLayout(pipeline->device->logical, pipelineLayoutInfo, nullptr, &pipeline->pipelineConfig->pipelineLayout) !=
            VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    delete pipelineLayoutInfo;
}

static void createRenderPass(GraphicsPipeline *pipeline)
{
    pipeline->pipelineConfig->renderPass = {};
    auto renderPassInfo = config::renderPassCreateInfo(


}
void bindPipeline(GraphicsPipeline* pipeline, VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
}

}
