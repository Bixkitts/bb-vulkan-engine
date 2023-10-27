#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE

#include <vulkan/vulkan_core.h>

#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"

//a default configuration for the pipeline containing all the info
//I'm going to need
PipelineConfig *pipelineConfigDefault(SwapChain *swapchain, std::vector<UniformBuffer*> &uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, std::vector<VkDescriptorSet> &descriptorSets);

VkGraphicsPipelineCreateInfo *createPipelineCreateInfo(PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo );

VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(PipelineConfig *config);
VkRenderPassCreateInfo *renderPassCreateInfo(SwapChain *swapchain);

VkShaderModuleCreateInfo shaderModuleInfo(const std::vector<char> &code);
VkPipelineVertexInputStateCreateInfo *vertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> *bindingDescriptions, std::vector<VkVertexInputAttributeDescription> *attributeDescriptions);
VkPipelineViewportStateCreateInfo *viewportCreateInfo(PipelineConfig *configInfo);
VkPipelineShaderStageCreateInfo *shaderStagesCreateInfo(GraphicsPipeline *mainPipeline);

#endif
