#include "pipeline.hpp"
#include "config_pipeline.hpp"
#include "error_handling.h"
#include "vertex.hpp"
#include <vulkan/vulkan_core.h>

//Make shader Modules
static BBError createVertShaderModule (GraphicsPipeline pipeline, 
                                       const std::vector<char>& code);
static BBError createFragShaderModule (GraphicsPipeline pipeline, 
                                       const std::vector<char>& code);
static void    cleanupShaderModules   (GraphicsPipeline pipeline);

BBError createGraphicsPipeline (GraphicsPipeline pipeline,
                                Device device,
                                SwapChain swapchain,
                                const std::string &vertFilepath, 
                                const std::string &fragFilepath,
                                PipelineConfig configInfo)
{
    // TODO: MALLOC without free
    pipeline = (GraphicsPipeline)calloc(1, sizeof(GraphicsPipeline_T));
    if (pipeline == NULL) {
        return BB_ERROR_MEM;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo      = {};
    VkPipelineShaderStageCreateInfo      shaderStageCreateInfo      = {};
    VkPipelineViewportStateCreateInfo    viewportStateCreateInfo    = {};
    VkGraphicsPipelineCreateInfo         pipelineCreateInfo         = {};
    VertexInputBindingDescriptions       inputBindingDescriptions   = {0};
    VertexInputAttributeDescriptions     inputAttributeDescriptions = {0};
    pipeline->device         = device;
    pipeline->swapchain      = swapchain;
    pipeline->pipelineConfig = configInfo;
    BBError er = BB_ERROR_UNKNOWN;

    // TODO: stdlib shit
    const std::vector<char> vertCode = readFile(vertFilepath);
    const std::vector<char> fragCode = readFile(fragFilepath);

    getVertexInputBindingDescriptions   (&inputBindingDescriptions);
    getVertexInputAttributeDescriptions (&inputAttributeDescriptions);

    if (configInfo->pipelineLayout != VK_NULL_HANDLE){
        er = BB_ERROR_PIPELINE_CREATE;
        goto error_exit;
    }
    if (configInfo->renderPass != VK_NULL_HANDLE){
        er = BB_ERROR_PIPELINE_CREATE;
        goto error_exit;
    }

    er = createVertShaderModule(pipeline, vertCode);    
    if (er != BB_ERROR_OK){
        goto error_exit;
    }
    er = createFragShaderModule(pipeline, fragCode);
    if (er != BB_ERROR_OK){
        vkDestroyShaderModule(pipeline->device->logical, pipeline->vertShaderModule, NULL);
        goto error_exit;
    }

    createVertexInputStateCreateInfo (&vertexInputCreateInfo, 
                                      &inputBindingDescriptions, 
                                      &inputAttributeDescriptions);
    createShaderStagesCreateInfo     (&shaderStageCreateInfo, 
                                      pipeline);
    createViewportCreateInfo         (&viewportStateCreateInfo, 
                                      configInfo);
    createPipelineCreateInfo         (&pipelineCreateInfo,
                                      configInfo,
                                      &viewportStateCreateInfo,
                                      &shaderStageCreateInfo,
                                      &vertexInputCreateInfo);
    if(
    vkCreateGraphicsPipelines        (device->logical,
                                      VK_NULL_HANDLE,
                                      1,
                                      &pipelineCreateInfo,
                                      NULL,
                                      &pipeline->graphicsPipeline) 
    != VK_SUCCESS){
        er = BB_ERROR_PIPELINE_CREATE;
        goto error_exit;
    }
    cleanupShaderModules(pipeline);
    return BB_ERROR_OK;

error_exit:
    free(pipeline);
    pipeline = NULL;
    return er;
}

// TODO: make sure this actually destroys everything
void destroyPipeline(GraphicsPipeline pipeline)
{
    vkDestroyPipelineLayout      (pipeline->device->logical, 
                                  pipeline->pipelineConfig->pipelineLayout, 
                                  NULL);
    vkDestroyDescriptorSetLayout (pipeline->device->logical, 
                                  pipeline->pipelineConfig->descriptorSetLayout, 
                                  NULL);
    vkDestroyPipeline            (pipeline->device->logical, 
                                  pipeline->graphicsPipeline, 
                                  NULL);
    free                         (pipeline->pipelineConfig);
    free                         (pipeline);
}
 
static void cleanupShaderModules(GraphicsPipeline pipeline)
{
    vkDestroyShaderModule(pipeline->device->logical, pipeline->vertShaderModule, nullptr);
    vkDestroyShaderModule(pipeline->device->logical, pipeline->fragShaderModule, nullptr);
}

static BBError createVertShaderModule(GraphicsPipeline pipeline, const std::vector<char>& code)
{
    pipeline->vertShaderModule = {};
    VkShaderModuleCreateInfo createInfo = {};
    createShaderModuleCreateInfo(&createInfo, code);

    if (vkCreateShaderModule(pipeline->device->logical, &createInfo, nullptr, &pipeline->vertShaderModule) 
    != VK_SUCCESS){
        fprintf(stderr, "\nfailed to create shader module");
        return BB_ERROR_SHADER_MODULE;
    }
    return BB_ERROR_OK;
}

static BBError createFragShaderModule(GraphicsPipeline pipeline, const std::vector<char>& code)
{
    pipeline->fragShaderModule = {};
    VkShaderModuleCreateInfo createInfo = {};
    createShaderModuleCreateInfo(&createInfo, code);

    if (vkCreateShaderModule(pipeline->device->logical, &createInfo, nullptr, &pipeline->fragShaderModule) != VK_SUCCESS) {
        fprintf(stderr, "\nfailed to create shader module");        
        return BB_ERROR_SHADER_MODULE;
    }
    return BB_ERROR_OK;
}

BBError createPipelineLayout(VkPipelineLayout *pipelineLayout, 
                             const Device device, 
                             const VkDescriptorSetLayout *descriptorSetLayout)
{
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
    createInfo.setLayoutCount          = 1;
    createInfo.pSetLayouts             = descriptorSetLayout;
    createInfo.pushConstantRangeCount  = 0; 
    createInfo.pPushConstantRanges     = NULL;

    if (vkCreatePipelineLayout(device->logical, &createInfo, NULL, pipelineLayout) 
        != VK_SUCCESS){
        fprintf(stderr, "\nfailed to create pipeline layout");
        return BB_ERROR_PIPELINE_LAYOUT_CREATE;
    }
    return BB_ERROR_OK;
}

void bindPipeline(GraphicsPipeline pipeline, VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
}

BBError createPipelineConfig(PipelineConfig config,
                             const SwapChain swapchain, 
                             UniformBuffer *uniformBuffers, 
                             const VkDescriptorSetLayout descriptorSetLayout, 
                             VkDescriptorSet *descriptorSets,
                             const VkPipelineLayout pipelineLayout)
{
    // TODO: MALLOC without free
    config = (PipelineConfig)calloc(1, sizeof(PipelineConfig));
    if (config == NULL){
        return BB_ERROR_MEM;
    }
    
    uint32_t width                                       = swapchain->swapChainExtent.width;
    uint32_t height                                      = swapchain->swapChainExtent.height;

    config->inputAssemblyInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config->inputAssemblyInfo.topology                   = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config->inputAssemblyInfo.primitiveRestartEnable     = VK_FALSE;
    config->inputAssemblyInfo.flags                      = 0;
    config->inputAssemblyInfo.pNext                      = NULL;

    config->viewport.x                                   = 0.0f;
    config->viewport.y                                   = 0.0f;
    // TODO: replace cast
    config->viewport.width                               = static_cast<float>(width);
    config->viewport.height                              = static_cast<float>(height);
    config->viewport.minDepth                            = 0.0f;
    config->viewport.maxDepth                            = 1.0f;

    config->scissor.offset                               = {0, 0};
    config->scissor.extent                               = {width, height};

    config->rasterizationInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config->rasterizationInfo.depthClampEnable           = VK_FALSE;
    config->rasterizationInfo.rasterizerDiscardEnable    = VK_FALSE;
    config->rasterizationInfo.polygonMode                = VK_POLYGON_MODE_FILL;
    config->rasterizationInfo.lineWidth                  = 1.0f;
    config->rasterizationInfo.cullMode                   = VK_CULL_MODE_BACK_BIT;
    config->rasterizationInfo.frontFace                  = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    config->rasterizationInfo.depthBiasEnable            = VK_FALSE;
    config->rasterizationInfo.depthBiasConstantFactor    = 0.0f;  // Optional
    config->rasterizationInfo.depthBiasClamp             = 0.0f;           // Optional
    config->rasterizationInfo.depthBiasSlopeFactor       = 0.0f;     // Optional

    config->multisampleInfo.sType                        = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config->multisampleInfo.sampleShadingEnable          = VK_FALSE;
    config->multisampleInfo.rasterizationSamples         = VK_SAMPLE_COUNT_1_BIT;
    config->multisampleInfo.minSampleShading             = 1.0f;           // Optional
    config->multisampleInfo.pSampleMask                  = NULL;             // Optional
    config->multisampleInfo.alphaToCoverageEnable        = VK_FALSE;  // Optional
    config->multisampleInfo.alphaToOneEnable             = VK_FALSE;       // Optional
    config->multisampleInfo.pNext                        = NULL;
    config->multisampleInfo.flags                        = 0;

    config->colorBlendAttachment.colorWriteMask          =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;
    config->colorBlendAttachment.blendEnable             = VK_FALSE;
    config->colorBlendAttachment.srcColorBlendFactor     = VK_BLEND_FACTOR_ONE;   // Optional
    config->colorBlendAttachment.dstColorBlendFactor     = VK_BLEND_FACTOR_ZERO;  // Optional
    config->colorBlendAttachment.colorBlendOp            = VK_BLEND_OP_ADD;              // Optional
    config->colorBlendAttachment.srcAlphaBlendFactor     = VK_BLEND_FACTOR_ONE;   // Optional
    config->colorBlendAttachment.dstAlphaBlendFactor     = VK_BLEND_FACTOR_ZERO;  // Optional
    config->colorBlendAttachment.alphaBlendOp            = VK_BLEND_OP_ADD;              // Optional

    config->colorBlendInfo.sType                         = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config->colorBlendInfo.logicOpEnable                 = VK_FALSE;
    config->colorBlendInfo.logicOp                       = VK_LOGIC_OP_COPY;  // Optional
    config->colorBlendInfo.attachmentCount               = 1;
    config->colorBlendInfo.pAttachments                  = &config->colorBlendAttachment;
    config->colorBlendInfo.blendConstants[0]             = 0.0f;  // Optional
    config->colorBlendInfo.blendConstants[1]             = 0.0f;  // Optional
    config->colorBlendInfo.blendConstants[2]             = 0.0f;  // Optional
    config->colorBlendInfo.blendConstants[3]             = 0.0f;  // Optional

    config->depthStencilInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config->depthStencilInfo.depthTestEnable             = VK_TRUE;
    config->depthStencilInfo.depthWriteEnable            = VK_TRUE;
    config->depthStencilInfo.depthCompareOp              = VK_COMPARE_OP_LESS;
    config->depthStencilInfo.depthBoundsTestEnable       = VK_FALSE;
    config->depthStencilInfo.minDepthBounds              = 0.0f;  // Optional
    config->depthStencilInfo.maxDepthBounds              = 1.0f;  // Optional
    config->depthStencilInfo.stencilTestEnable           = VK_FALSE;
    config->depthStencilInfo.front                       = {};  // Optional
    config->depthStencilInfo.back                        = {};   // Optional
    config->depthStencilInfo.pNext                       = NULL;

    config->renderPass                                   = swapchain->renderPass;

    config->uniformBuffers                               = uniformBuffers;

    //Creating layouts and descriptors
    config->descriptorSetLayout                          = descriptorSetLayout;
    config->descriptorSets                               = descriptorSets;

    config->pipelineLayout                               = pipelineLayout;
    return BB_ERROR_OK;
}
