#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE

#include <vulkan/vulkan_core.h>

#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"

BBError createPipelineCreateInfo(VkGraphicsPipelineCreateInfo createInfo, PipelineConfig *configInfo, VkPipelineViewportStateCreateInfo *viewportInfo,VkPipelineShaderStageCreateInfo *shaderStages,VkPipelineVertexInputStateCreateInfo *vertexInputInfo );

BBError pipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo *createInfo, PipelineConfig *config);
VkRenderPassCreateInfo *renderPassCreateInfo(SwapChain *swapchain);

VkShaderModuleCreateInfo shaderModuleInfo(const std::vector<char> &code);
VkPipelineVertexInputStateCreateInfo *vertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> *bindingDescriptions, std::vector<VkVertexInputAttributeDescription> *attributeDescriptions);
VkPipelineViewportStateCreateInfo *viewportCreateInfo(PipelineConfig *configInfo);
VkPipelineShaderStageCreateInfo *shaderStagesCreateInfo(GraphicsPipeline *mainPipeline);

#endif
