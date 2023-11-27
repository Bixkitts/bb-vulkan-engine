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

GraphicsPipeline* createGraphicsPipeline(GraphicsPipeline *pipeline,
                                         Device *device,
                                         SwapChain *swapchain,
                                         const std::string &vertFilepath, 
                                         const std::string &fragFilepath,
                                         const PipelineConfig *configInfo)
{
    pipeline = (GraphicsPipeline*)calloc(1, sizeof(GraphicsPipeline));

    // TODO: a whole bunch of stdlib shit
    assert(configInfo->pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
    assert(configInfo->renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");
    const std::vector<char> vertCode = readFile(vertFilepath);
    const std::vector<char> fragCode = readFile(fragFilepath);
    std::cout<< "Vertex Shader Code Size: " << vertCode.size() << '\n';
    std::cout<< "Fragment Shader Code Size: " << fragCode.size() << '\n';

    pipeline->device = device;
    pipeline->swapchain = swapchain;
    createVertShaderModule(pipeline, vertCode);             //These create the VkShaderModules
    createFragShaderModule(pipeline, fragCode);             //stored in the GraphicsPipeline
    
    pipeline->pipelineConfig = configInfo;       //The default configuration I've passed in
                                                                //stays stored in the pipeline object 
                                                                //for reference and vulkan deallocation

    // Here the vertex input info is put constructed 
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

BBError createPipelineConfig(PipelineConfig *config,
                             const SwapChain *swapchain, 
                             const UniformBuffer *uniformBuffers, 
                             const VkDescriptorSetLayout descriptorSetLayout, 
                             const VkDescriptorSet *descriptorSets)
{
    // TODO: MALLOC without free
    config = (PipelineConfig*)calloc(1, sizeof(PipelineConfig));
    
    uint32_t width                               = swapchain->swapChainExtent.width;
    uint32_t height                              = swapchain->swapChainExtent.height;

    config->inputAssemblyInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config->inputAssemblyInfo.topology                   = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config->inputAssemblyInfo.primitiveRestartEnable     = VK_FALSE;
    config->inputAssemblyInfo.flags                      = 0;
    config->inputAssemblyInfo.pNext                      = NULL;

    config->viewport.x                                   = 0.0f;
    config->viewport.y                                   = 0.0f;
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
    config->multisampleInfo.pSampleMask                  = nullptr;             // Optional
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

    config->depthStencilInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config->depthStencilInfo.depthTestEnable            = VK_TRUE;
    config->depthStencilInfo.depthWriteEnable           = VK_TRUE;
    config->depthStencilInfo.depthCompareOp             = VK_COMPARE_OP_LESS;
    config->depthStencilInfo.depthBoundsTestEnable      = VK_FALSE;
    config->depthStencilInfo.minDepthBounds             = 0.0f;  // Optional
    config->depthStencilInfo.maxDepthBounds             = 1.0f;  // Optional
    config->depthStencilInfo.stencilTestEnable          = VK_FALSE;
    config->depthStencilInfo.front                      = {};  // Optional
    config->depthStencilInfo.back                       = {};   // Optional
    config->depthStencilInfo.pNext                      = NULL;

    config->renderPass                                  = swapchain->renderPass;

    // Oh dang this is important, uniform buffers!
    config->uniformBuffers                              = uniformBuffers;

    //Creating layouts and descriptors
    config->descriptorSetLayout  = descriptorSetLayout;
    config->descriptorSets       = descriptorSets;

    config->pipelineLayout       = createPipelineLayout(swapchain->device, config);

    return BB_ERROR_OK;
}
