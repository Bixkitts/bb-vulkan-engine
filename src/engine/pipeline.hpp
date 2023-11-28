#ifndef BVE_PIPELINE
#define BVE_PIPELINE

#include <vector>
#include <string>

#include <vulkan/vulkan_core.h>

#include "buffers.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"
#include "error_handling.h"
#include "fileIO.hpp"

typedef struct PipelineConfig
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
} PipelineConfig;
typedef struct GraphicsPipeline
{
    Device          device;
    SwapChain      *swapchain;
    VkPipeline      graphicsPipeline;
    VkShaderModule  vertShaderModule;
    VkShaderModule  fragShaderModule;
    PipelineConfig *pipelineConfig;
} GraphicsPipeline;

void              bindPipeline           (GraphicsPipeline* pipeline, 
                                          VkCommandBuffer commandBuffer);
GraphicsPipeline *createGraphicsPipeline (GraphicsPipeline *pipeline,
                                          const Device device,
                                          const SwapChain* swapchain,
                                          const std::string& vertFilepath, 
                                          const std::string& fragFilepath,
                                          const PipelineConfig* configInfo);
void              destroyPipeline        (GraphicsPipeline* pipeline);
BBError           createPipelineLayout   (VkPipelineLayout *layout, 
                                          Device device, 
                                          PipelineConfig *config);
BBError           createPipelineConfig   (PipelineConfig *config,
                                          const SwapChain *swapchain, 
                                          const UniformBuffer *uniformBuffers, 
                                          const VkDescriptorSetLayout descriptorSetLayout, 
                                          const VkDescriptorSet *descriptorSets);

#endif
