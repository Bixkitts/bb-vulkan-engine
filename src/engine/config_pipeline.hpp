#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include <vulkan/vulkan_core.h>
namespace config
{

    bve::PipelineConfig *pipelineConfigDefault(bve::SwapChain *swapchain);

    VkGraphicsPipelineCreateInfo *pipelineInfo(bve::PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo );

    VkPipelineLayoutCreateInfo *pipelineLayoutInfo();

    VkShaderModuleCreateInfo *shaderModuleInfo(const std::vector<char> &code);
    VkPipelineVertexInputStateCreateInfo *vertexInputInfo(std::vector<VkVertexInputBindingDescription> *bindingDescriptions, std::vector<VkVertexInputAttributeDescription> *attributeDescriptions);
    VkPipelineViewportStateCreateInfo *viewportInfo(bve::PipelineConfig *configInfo);
    VkPipelineShaderStageCreateInfo *createShaderStages(bve::GraphicsPipeline *mainPipeline);
}


#endif
