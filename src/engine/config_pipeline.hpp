#ifndef CONFIG_PIPELINE
#define CONFIG_PIPELINE

#include <vulkan/vulkan_core.h>

#include "buffers.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"

void    createPipelineCreateInfo         (VkGraphicsPipelineCreateInfo *createInfo, 
                                          PipelineConfig *configInfo, 
                                          VkPipelineViewportStateCreateInfo *viewportInfo,
                                          VkPipelineShaderStageCreateInfo *shaderStages,
                                          VkPipelineVertexInputStateCreateInfo *vertexInputInfo );
void    createPipelineLayoutCreateInfo   (VkPipelineLayoutCreateInfo *createInfo, 
                                          PipelineConfig *config);
void    createShaderModuleCreateInfo     (VkShaderModuleCreateInfo *shadermodCreateInfo, 
                                          const std::vector<char> &code);
void    createVertexInputStateCreateInfo (VkPipelineVertexInputStateCreateInfo *vertexInputCreateInfo,
                                          VkVertexInputBindingDescription *bindingDescriptions, 
                                          uint32_t bindingDescriptionCount,
                                          VkVertexInputAttributeDescription *attributeDescriptions,
                                          uint32_t attributeDescriptionCount);
void    createViewportCreateInfo         (VkPipelineViewportStateCreateInfo *viewportCreateInfo, 
                                          PipelineConfig *configInfo);
void    createShaderStagesCreateInfo     (VkPipelineShaderStageCreateInfo *shaderStageCreateInfo, 
                                          GraphicsPipeline *mainPipeline);
#endif
