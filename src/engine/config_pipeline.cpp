#include "config_pipeline.hpp"
#include <vulkan/vulkan_core.h>

BBError createPipelineCreateInfo(VkGraphicsPipelineCreateInfo *createInfo, PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo  )
{
    createInfo->sType                     = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // TODO: magic number 2
    createInfo->stageCount                = 2;
    createInfo->pStages                   = shaderStages;
    createInfo->pVertexInputState         = vertexInputInfo;
    createInfo->pInputAssemblyState       = &configInfo->inputAssemblyInfo;
    createInfo->pViewportState            = viewportInfo;
    createInfo->pRasterizationState       = &configInfo->rasterizationInfo;
    createInfo->pMultisampleState         = &configInfo->multisampleInfo;
    createInfo->pColorBlendState          = &configInfo->colorBlendInfo;
    createInfo->pDepthStencilState        = &configInfo->depthStencilInfo;
    createInfo->pDynamicState             = NULL;
    createInfo->layout                    = configInfo->pipelineLayout;
    createInfo->renderPass                = configInfo->renderPass;
    createInfo->subpass                   = configInfo->subpass;
    createInfo->basePipelineIndex         = -1;
    createInfo->basePipelineHandle        = VK_NULL_HANDLE;
    return BB_ERROR_OK;
}
BBError pipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo *createInfo, PipelineConfig *config)
{
    createInfo->sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
    createInfo->setLayoutCount          = 1;
    createInfo->pSetLayouts             = &config->descriptorSetLayout;
    createInfo->pushConstantRangeCount  = 0; 
    createInfo->pPushConstantRanges     = nullptr;
    return BB_ERROR_OK;
}
VkShaderModuleCreateInfo shaderModuleInfo(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType       = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize    = code.size();
    createInfo.pCode       = reinterpret_cast<const uint32_t*>(code.data());
    return createInfo;
}
VkPipelineVertexInputStateCreateInfo *vertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> *bindingDescriptions, std::vector<VkVertexInputAttributeDescription> *attributeDescriptions)
{
    auto *vertexInputInfo                               = new VkPipelineVertexInputStateCreateInfo{};
    vertexInputInfo->sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo->vertexAttributeDescriptionCount    = static_cast<uint32_t>(attributeDescriptions->size());
    vertexInputInfo->vertexBindingDescriptionCount      = static_cast<uint32_t>(bindingDescriptions->size());
    vertexInputInfo->pVertexAttributeDescriptions       = attributeDescriptions->data();
    vertexInputInfo->pVertexBindingDescriptions         = bindingDescriptions->data();
    return vertexInputInfo;
}
VkPipelineViewportStateCreateInfo *viewportCreateInfo(PipelineConfig *configInfo)
{
    auto *viewportInfo          = new VkPipelineViewportStateCreateInfo{};
    viewportInfo->sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo->viewportCount = 1;
    viewportInfo->pViewports    = &configInfo->viewport;
    viewportInfo->scissorCount  = 1;
    viewportInfo->pScissors     = &configInfo->scissor;
    return viewportInfo;
}
VkPipelineShaderStageCreateInfo *shaderStagesCreateInfo(GraphicsPipeline *mainPipeline)
{
    auto *shaderStages      = new VkPipelineShaderStageCreateInfo[2];
    shaderStages[0].sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage   = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module  = mainPipeline->vertShaderModule;
    shaderStages[0].pName   = "main";
    shaderStages[0].flags   = 0;
    shaderStages[0].pNext   = nullptr;
    shaderStages[0].
        pSpecializationInfo = nullptr;
    shaderStages[1].sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage   = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module  = mainPipeline->fragShaderModule;
    shaderStages[1].pName   = "main";
    shaderStages[1].flags   = 0;
    shaderStages[1].pNext   = nullptr;
    shaderStages[1].
        pSpecializationInfo = nullptr;
    return shaderStages;
}
