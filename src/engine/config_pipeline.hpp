#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE
#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include <vulkan/vulkan_core.h>
namespace config
{
    //a default configuration for the pipeline containing all the info
    //I'm going to need
    bve::PipelineConfig *pipelineConfigDefault(bve::SwapChain *swapchain, std::vector<bve::UniformBuffer*> &uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, std::vector<VkDescriptorSet> &descriptorSets);

    VkGraphicsPipelineCreateInfo *pipelineCreateInfo(bve::PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo );

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(bve::PipelineConfig *config);
    VkRenderPassCreateInfo *renderPassCreateInfo(bve::SwapChain *swapchain);

    VkShaderModuleCreateInfo shaderModuleInfo(const std::vector<char> &code);
    VkPipelineVertexInputStateCreateInfo *vertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> *bindingDescriptions, std::vector<VkVertexInputAttributeDescription> *attributeDescriptions);
    VkPipelineViewportStateCreateInfo *viewportCreateInfo(bve::PipelineConfig *configInfo);
    VkPipelineShaderStageCreateInfo *shaderStagesCreateInfo(bve::GraphicsPipeline *mainPipeline);
}


#endif
