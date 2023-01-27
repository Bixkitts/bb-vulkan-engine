#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include <vulkan/vulkan_core.h>
namespace config
{
    //a default configuration for the pipeline containing all the info
    //I'm going to need
    bve::PipelineConfig *pipelineConfigDefault(bve::SwapChain *swapchain);

    VkGraphicsPipelineCreateInfo *pipelineCreateInfo(bve::PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo );

    VkPipelineLayoutCreateInfo *pipelineLayoutCreateInfo(VkDescriptorSetLayout *descriptorSetLayout);
    VkRenderPassCreateInfo *renderPassCreateInfo(bve::SwapChain *swapchain);

    VkShaderModuleCreateInfo *shaderModuleInfo(const std::vector<char> &code);
    VkPipelineVertexInputStateCreateInfo *vertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> *bindingDescriptions, std::vector<VkVertexInputAttributeDescription> *attributeDescriptions);
    VkPipelineViewportStateCreateInfo *viewportCreateInfo(bve::PipelineConfig *configInfo);
    VkPipelineShaderStageCreateInfo *shaderStagesCreateInfo(bve::GraphicsPipeline *mainPipeline);
}


#endif
