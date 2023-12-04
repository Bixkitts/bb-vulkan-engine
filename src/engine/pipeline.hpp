#ifndef BB_PIPELINE
#define BB_PIPELINE

#include <string>

#include <vulkan/vulkan_core.h>

#include "descriptor_sets.hpp"
#include "buffers.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"
#include "error_handling.h"
#include "fileIO.hpp"

typedef struct PipelineConfig_S
{
    //TODO: data alignment
    VkViewport                             viewport;
    VkRect2D                               scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo   multisampleInfo;
    VkPipelineColorBlendAttachmentState    colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;

    //TODO: Vk objects that need to be properly destroyed!
    VkDescriptorSetLayout                  descriptorSetLayout;
    VkDescriptorSet                       *descriptorSets;
    uint64_t                               descriptorSetCount;

    VkPipelineLayout                       pipelineLayout;
    VkRenderPass                           renderPass;

    UniformBuffer                         *uniformBuffers;     
    uint64_t                               uniformBufferCount;
    // There are subpasses created and
    // configured in the RenderPass     
    uint32_t                               subpass = 0; 
} PipelineConfig_T, *PipelineConfig;

typedef struct GraphicsPipeline_S
{
    Device          device;
    SwapChain       swapchain;
    VkPipeline      graphicsPipeline;
    VkShaderModule  vertShaderModule;
    VkShaderModule  fragShaderModule;
    PipelineConfig  pipelineConfig;
} GraphicsPipeline_T, *GraphicsPipeline;

void              bindPipeline           (GraphicsPipeline pipeline, 
                                          VkCommandBuffer commandBuffer);
BBError           createGraphicsPipeline (GraphicsPipeline pipeline,
                                          const Device device,
                                          const SwapChain swapchain,
                                          const std::string& vertFilepath, 
                                          const std::string& fragFilepath,
                                          const PipelineConfig configInfo);
void              destroyPipeline        (GraphicsPipeline pipeline);
BBError           createPipelineLayout   (VkPipelineLayout *layout, 
                                          const Device device, 
                                          const VkDescriptorSetLayout *descriptorSetLayout);
BBError           createPipelineConfig   (PipelineConfig *config,
                                          const SwapChain swapchain, 
                                          const UniformBufferArray uniformBuffers, 
                                          const VkDescriptorSetLayout descriptorSetLayout, 
                                          const VkDescriptorSetArray descriptorSets,
                                          const VkPipelineLayout pipelineLayout);

#endif
