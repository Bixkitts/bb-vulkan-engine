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

        GraphicsPipeline* mainPipeline = new GraphicsPipeline{};
        mainPipeline->myPipelineDevice = device;

        mainPipeline->vertShaderModule = createShaderModule(device, vertCode);
        mainPipeline->fragShaderModule = createShaderModule(device, fragCode);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = mainPipeline->vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = mainPipeline->fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;
        
        auto bindingDescriptions = getBindingDescriptions();
        auto attributeDescriptions = getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &configInfo->viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &configInfo->scissor;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        #include "configs/pipeline/pipelineCreateInfo.conf"
        if(vkCreateGraphicsPipelines(device->logical, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mainPipeline->graphicsPipeline) != VK_SUCCESS)
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
        PipelineConfig* config = config::pipelineConfigDefault();
        return config;
    }

    VkShaderModule createShaderModule(Device* device, const std::vector<char>& code)
    {
        VkShaderModule shaderModule{};
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());


        if (vkCreateShaderModule(device->logical, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module \n");        
        }

        return shaderModule;
    }

    VkPipelineLayout createPipelineLayout(Device* device)
    {
        VkPipelineLayout pipelineLayout{};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        #include "configs/pipeline/pipelineLayoutInfo.conf"

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

}
