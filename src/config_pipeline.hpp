#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
namespace config
{
    bve::PipelineConfig *pipelineConfigDefault(bve::SwapChain *swapchain);
    VkGraphicsPipelineCreateInfo *pipelineInfo(bve::PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo );

}


#endif
