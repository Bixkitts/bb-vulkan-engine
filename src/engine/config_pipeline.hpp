#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE

#include <vulkan/vulkan_core.h>

#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"

//a default configuration for the pipeline containing all the info
//I'm going to need
BBError createDefaultPipelineConfig(PipelineConfig *config, SwapChain *swapchain, UniformBuffer *uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet *descriptorSets);

BBError createPipelineCreateInfo(VkGraphicsPipelineCreateInfo createInfo, PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo );

BBError pipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo *createInfo, PipelineConfig *config);
VkRenderPassCreateInfo *renderPassCreateInfo(SwapChain *swapchain);

VkShaderModuleCreateInfo shaderModuleInfo(const std::vector<char> &code);
VkPipelineVertexInputStateCreateInfo *vertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> *bindingDescriptions, std::vector<VkVertexInputAttributeDescription> *attributeDescriptions);
VkPipelineViewportStateCreateInfo *viewportCreateInfo(PipelineConfig *configInfo);
VkPipelineShaderStageCreateInfo *shaderStagesCreateInfo(GraphicsPipeline *mainPipeline);

#endif
