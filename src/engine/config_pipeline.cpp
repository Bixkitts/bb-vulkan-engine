#include "config_pipeline.hpp"
#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include <vulkan/vulkan_core.h>
namespace config
{
    bve::PipelineConfig *pipelineConfigDefault(bve::SwapChain *swapchain, std::vector<bve::UniformBuffer*> &uniformBuffers)
    {
        auto *config                                 = new bve::PipelineConfig{};
        
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

        config->uniformBuffers                              = uniformBuffers;

        //Creating layouts and descriptors
        config->descriptorSetLayout  = bve::createDescriptorSetLayout(swapchain->device);
        config->descriptorPool       = bve::createDescriptorPool(swapchain->device);
        config->descriptorSets       = bve::createDescriptorSets(swapchain->device, config);

        config->pipelineLayout       = bve::createPipelineLayout(swapchain->device, config);
        return config;
    }
    VkGraphicsPipelineCreateInfo *pipelineCreateInfo(bve::PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo  )
    {
        auto *pipelineInfo                      = new VkGraphicsPipelineCreateInfo{};
        pipelineInfo->sType                     = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo->stageCount                = 2;
        pipelineInfo->pStages                   = shaderStages;
        pipelineInfo->pVertexInputState         = vertexInputInfo;
        pipelineInfo->pInputAssemblyState       = &configInfo->inputAssemblyInfo;
        pipelineInfo->pViewportState            = viewportInfo;
        pipelineInfo->pRasterizationState       = &configInfo->rasterizationInfo;
        pipelineInfo->pMultisampleState         = &configInfo->multisampleInfo;
        pipelineInfo->pColorBlendState          = &configInfo->colorBlendInfo;
        pipelineInfo->pDepthStencilState        = &configInfo->depthStencilInfo;
        pipelineInfo->pDynamicState             = nullptr;
        pipelineInfo->layout                    = configInfo->pipelineLayout;
        pipelineInfo->renderPass                = configInfo->renderPass;
        pipelineInfo->subpass                   = configInfo->subpass;
        pipelineInfo->basePipelineIndex         = -1;
        pipelineInfo->basePipelineHandle        = VK_NULL_HANDLE;
        return pipelineInfo;
    }
    VkPipelineLayoutCreateInfo *pipelineLayoutCreateInfo(bve::PipelineConfig *config)
    {
        auto *pipelineLayoutInfo                    = new VkPipelineLayoutCreateInfo{};
        pipelineLayoutInfo->sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
        pipelineLayoutInfo->setLayoutCount          = 1;
        pipelineLayoutInfo->pSetLayouts             = &config->descriptorSetLayout;
        pipelineLayoutInfo->pushConstantRangeCount  = 0; 
        pipelineLayoutInfo->pPushConstantRanges     = nullptr;
        return pipelineLayoutInfo;
    }
    VkShaderModuleCreateInfo *shaderModuleInfo(const std::vector<char> &code)
    {
        auto *createInfo        = new VkShaderModuleCreateInfo{};
        createInfo->sType       = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo->codeSize    = code.size();
        createInfo->pCode       = reinterpret_cast<const uint32_t*>(code.data());
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
    VkPipelineViewportStateCreateInfo *viewportCreateInfo(bve::PipelineConfig *configInfo)
    {
        auto *viewportInfo          = new VkPipelineViewportStateCreateInfo{};
        viewportInfo->sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo->viewportCount = 1;
        viewportInfo->pViewports    = &configInfo->viewport;
        viewportInfo->scissorCount  = 1;
        viewportInfo->pScissors     = &configInfo->scissor;
        return viewportInfo;
    }
    VkPipelineShaderStageCreateInfo *shaderStagesCreateInfo(bve::GraphicsPipeline *mainPipeline)
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
}
