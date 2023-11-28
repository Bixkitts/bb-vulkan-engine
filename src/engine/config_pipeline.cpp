#include "config_pipeline.hpp"

void createPipelineCreateInfo(VkGraphicsPipelineCreateInfo *createInfo, 
                                 PipelineConfig *configInfo, 
                                 VkPipelineViewportStateCreateInfo *viewportInfo,
                                 VkPipelineShaderStageCreateInfo *shaderStages,
                                 VkPipelineVertexInputStateCreateInfo *vertexInputInfo  )
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
}
void createPipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo *createInfo, 
                                       PipelineConfig *config)
{
    createInfo->sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
    createInfo->setLayoutCount          = 1;
    createInfo->pSetLayouts             = &config->descriptorSetLayout;
    createInfo->pushConstantRangeCount  = 0; 
    createInfo->pPushConstantRanges     = nullptr;
}

// TODO: stdlib shit
void createShaderModuleCreateInfo(VkShaderModuleCreateInfo *createInfo, 
                                  const std::vector<char> &code)
{
    createInfo->sType       = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo->codeSize    = code.size();
    createInfo->pCode       = reinterpret_cast<const uint32_t*>(code.data());
}
void createVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo *createInfo, 
                                      VkVertexInputBindingDescription *bindingDescriptions, 
                                      uint32_t bindingDescriptionCount,
                                      VkVertexInputAttributeDescription *attributeDescriptions,
                                      uint32_t attributeDescriptionCount)
{
    createInfo->sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo->vertexAttributeDescriptionCount = attributeDescriptionCount;
    createInfo->vertexBindingDescriptionCount   = bindingDescriptionCount;
    createInfo->pVertexAttributeDescriptions    = attributeDescriptions;
    createInfo->pVertexBindingDescriptions      = bindingDescriptions;
}
void viewportCreateInfo(VkPipelineViewportStateCreateInfo *viewportCreateInfo, 
                        PipelineConfig *configInfo)
{
    viewportCreateInfo->sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportCreateInfo->viewportCount = 1;
    viewportCreateInfo->pViewports    = &configInfo->viewport;
    viewportCreateInfo->scissorCount  = 1;
    viewportCreateInfo->pScissors     = &configInfo->scissor;
}
void createShaderStagesCreateInfo(VkPipelineShaderStageCreateInfo *createInfo, 
                                  uint32_t shaderStageCount, 
                                  GraphicsPipeline *mainPipeline)
{
    createInfo[0].sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo[0].stage   = VK_SHADER_STAGE_VERTEX_BIT;
    createInfo[0].module  = mainPipeline->vertShaderModule;
    createInfo[0].pName   = "main";
    createInfo[0].flags   = 0;
    createInfo[0].pNext   = NULL;
    createInfo[0].
    pSpecializationInfo   = NULL;
    createInfo[1].sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo[1].stage   = VK_SHADER_STAGE_FRAGMENT_BIT;
    createInfo[1].module  = mainPipeline->fragShaderModule;
    createInfo[1].pName   = "main";
    createInfo[1].flags   = 0;
    createInfo[1].pNext   = NULL;
    createInfo[1].
    pSpecializationInfo   = NULL;
}
