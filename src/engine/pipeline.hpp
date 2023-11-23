#ifndef BVE_PIPELINE
#define BVE_PIPELINE

#include <vector>
#include <string>

#include <vulkan/vulkan_core.h>

#include "buffers.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

struct PipelineConfig
{
    //TODO: data alignment
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

    //TODO: Vk objects that need to be properly destroyed!
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet *descriptorSets;
    uint64_t descriptorSetCount;

    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;

    UniformBuffer *uniformBuffers; //pointer to the main vector of uniform Buffers.
    uint64_t uniformBufferCount;
    
    uint32_t subpass = 0; // There are subpasses created and 
                          // configured in the RenderPass
};
struct GraphicsPipeline
{
    Device *device;
    SwapChain *swapchain;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    PipelineConfig *pipelineConfig;
};

void bindPipeline(GraphicsPipeline* pipeline, VkCommandBuffer commandBuffer);

// TODO: std string and vector replacement
// and move this function to an IO translation unit
std::vector<char> readFile(const std::string& filepath);

PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain, std::vector<UniformBuffer*> &uniformBuffers, VkDescriptorSetLayout descriptorSetLayout, std::vector<VkDescriptorSet> &descriptorSets);

GraphicsPipeline* createGraphicsPipeline(
        Device* device,
        SwapChain* swapchain,
        const std::string& vertFilepath, 
        const std::string& fragFilepath,
        PipelineConfig* configInfo);

void destroyPipeline(GraphicsPipeline* pipeline);


VkPipelineLayout createPipelineLayout(Device *device, PipelineConfig *config);


#endif
