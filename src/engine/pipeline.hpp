#ifndef BVE_PIPELINE
#define BVE_PIPELINE

#include <vector>
#include <string>

#include <vulkan/vulkan_core.h>

#include "buffers.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

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

    //pointer to the main vector of uniform Buffers.
    UniformBuffer                         *uniformBuffers;     
    uint64_t                               uniformBufferCount;
    // There are subpasses created and
    // configured in the RenderPass     
    uint32_t                               subpass = 0; 
} PipelineConfig;
typedef struct GraphicsPipeline
{
    Device         *device;
    SwapChain      *swapchain;
    VkPipeline      graphicsPipeline;
    VkShaderModule  vertShaderModule;
    VkShaderModule  fragShaderModule;
    PipelineConfig *pipelineConfig;
} GraphicsPipeline;
void              bindPipeline           (GraphicsPipeline* pipeline, 
                                          VkCommandBuffer commandBuffer);
// TODO: std string and vector replacement
// and move this function to an IO translation unit
std::vector<char> readFile               (const std::string& filepath);
GraphicsPipeline* createGraphicsPipeline (GraphicsPipeline *pipeline,
                                          const Device* device,
                                          const SwapChain* swapchain,
                                          const std::string& vertFilepath, 
                                          const std::string& fragFilepath,
                                          const PipelineConfig* configInfo);
void              destroyPipeline        (GraphicsPipeline* pipeline);
VkPipelineLayout  createPipelineLayout   (Device *device, PipelineConfig *config);
BBError           createPipelineConfig   (PipelineConfig *config,
                                          const SwapChain *swapchain, 
                                          const UniformBuffer *uniformBuffers, 
                                          const VkDescriptorSetLayout descriptorSetLayout, 
                                          const VkDescriptorSet *descriptorSets);

#endif
