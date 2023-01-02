#include "bve_pipeline.hpp"
#include "bve_device.hpp"
#include "bve_model.hpp"
#include "bve_swap_chain.hpp"
#include "config_pipeline.hpp"

//std libraries
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vulkan/vulkan_core.h>
#include <cassert>
#include <stdexcept>

namespace bve
{

std::vector<char> readFile(const std::string& filepath)
{
    std::ifstream file{filepath, std::ios::ate | std::ios::binary};

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file :" + filepath);
    }        

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

GraphicsPipeline* createGraphicsPipeline(
        Device* device,
        const std::string& vertFilepath, 
        const std::string& fragFilepath,
        PipelineConfig* configInfo)
{
    assert(configInfo->pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
    assert(configInfo->renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");
    const std::vector<char> vertCode = readFile(vertFilepath);
    const std::vector<char> fragCode = readFile(fragFilepath);
    std::cout<< "Vertex Shader Code Size: " << vertCode.size() << '\n';
    std::cout<< "Fragment Shader Code Size: " << fragCode.size() << '\n';

    auto mainPipeline = new GraphicsPipeline{};

    mainPipeline->myPipelineDevice = device;
    mainPipeline->vertShaderModule = createShaderModule(device, vertCode);
    mainPipeline->fragShaderModule = createShaderModule(device, fragCode);
    auto shaderStages = config::createShaderStages(mainPipeline);
    auto bindingDescriptions = getBindingDescriptions();
    auto attributeDescriptions = getAttributeDescriptions();
    auto *vertexInputInfo = config::vertexInputInfo(&bindingDescriptions, &attributeDescriptions);
    auto *viewportInfo = config::viewportInfo(configInfo);
    auto *pipelineInfo = config::pipelineInfo(configInfo, viewportInfo,shaderStages, vertexInputInfo );
    if(vkCreateGraphicsPipelines(device->logical, VK_NULL_HANDLE, 1, pipelineInfo, nullptr, &mainPipeline->graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("pipeline not created successfully \n");
    }
    return mainPipeline;
}
 
    void destroyBveGraphicsPipeline(GraphicsPipeline* pipeline)
    {
        vkDestroyShaderModule(pipeline->myPipelineDevice->logical, pipeline->vertShaderModule, nullptr);
        vkDestroyShaderModule(pipeline->myPipelineDevice->logical, pipeline->fragShaderModule, nullptr);
    }

    PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain)
    {
        PipelineConfig *config = config::pipelineConfigDefault(swapchain);
        return config;
    }

    VkShaderModule createShaderModule(Device* device, const std::vector<char>& code)
    {
        VkShaderModule shaderModule{};
        auto createInfo = config::shaderModuleInfo(code);


        if (vkCreateShaderModule(device->logical, createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module \n");        
        }

        return shaderModule;
    }

    VkPipelineLayout createPipelineLayout(Device* device)
    {
        VkPipelineLayout pipelineLayout{};
        auto pipelineLayoutInfo = config::pipelineLayoutInfo();

        if(vkCreatePipelineLayout(device->logical, pipelineLayoutInfo, nullptr, &pipelineLayout) !=
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

}
