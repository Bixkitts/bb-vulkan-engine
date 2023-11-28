#include "config_pipeline.hpp"

void createPipelineCreateInfo(VkGraphicsPipelineCreateInfo *createInfo, 
                              PipelineConfig config, 
                              VkPipelineViewportStateCreateInfo *viewportInfo,
                              VkPipelineShaderStageCreateInfo *shaderStages,
                              VkPipelineVertexInputStateCreateInfo *vertexInputInfo  )
{
    createInfo->sType                     = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // TODO: magic number 2
    createInfo->stageCount                = 2;
    createInfo->pStages                   = shaderStages;
    createInfo->pVertexInputState         = vertexInputInfo;
    createInfo->pInputAssemblyState       = &config->inputAssemblyInfo;
    createInfo->pViewportState            = viewportInfo;
    createInfo->pRasterizationState       = &config->rasterizationInfo;
    createInfo->pMultisampleState         = &config->multisampleInfo;
    createInfo->pColorBlendState          = &config->colorBlendInfo;
    createInfo->pDepthStencilState        = &config->depthStencilInfo;
    createInfo->pDynamicState             = NULL;
    createInfo->layout                    = config->pipelineLayout;
    createInfo->renderPass                = config->renderPass;
    createInfo->subpass                   = config->subpass;
    createInfo->basePipelineIndex         = -1;
    createInfo->basePipelineHandle        = VK_NULL_HANDLE;
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
                                      VertexInputBindingDescriptions *bindingDescriptions, 
                                      VertexInputAttributeDescriptions *attributeDescriptions)
{
    createInfo->sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo->vertexAttributeDescriptionCount = attributeDescriptions->count;
    createInfo->vertexBindingDescriptionCount   = bindingDescriptions->count;
    createInfo->pVertexAttributeDescriptions    = attributeDescriptions->data;
    createInfo->pVertexBindingDescriptions      = bindingDescriptions->data;
}
void viewportCreateInfo(VkPipelineViewportStateCreateInfo *createInfo, 
                        PipelineConfig configInfo)
{
    createInfo->sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo->viewportCount = 1;
    createInfo->pViewports    = &configInfo->viewport;
    createInfo->scissorCount  = 1;
    createInfo->pScissors     = &configInfo->scissor;
}
void createShaderStagesCreateInfo(VkPipelineShaderStageCreateInfo *createInfo, 
                                  uint32_t shaderStageCount, 
                                  GraphicsPipeline mainPipeline)
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
