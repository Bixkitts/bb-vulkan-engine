#ifndef BB_PIPELINE
#define BB_PIPELINE

#include <string>

#include <vulkan/vulkan_core.h>

#include "defines.hpp"
#include "descriptor_sets.hpp"
#include "buffers.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "error_handling.h"

typedef struct PipelineConfig_T   PipelineConfig_T;
typedef struct GraphicsPipeline_T Graphicspipeline_T;

// A simple selector for any variety of pipeline layouts
// I may concievably need
typedef enum{
    PIPELINE_LAYOUT_BASIC,
    PIPELINE_LAYOUT_COUNT,
}PipelineLayoutEnum;

BB_OPAQUE_HANDLE(PipelineConfig);
BB_OPAQUE_HANDLE(GraphicsPipeline);

void                 bindPipeline               (GraphicsPipeline pipeline, 
                                                 VkCommandBuffer commandBuffer);
BBError              createGraphicsPipeline     (GraphicsPipeline pipeline,
                                                 const Device device,
                                                 const SwapChain swapchain,
                                                 const std::string& vertFilepath, 
                                                 const std::string& fragFilepath,
                                                 PipelineConfig configInfo);
void                 destroyPipeline            (GraphicsPipeline pipeline);
BBError              createPipelineLayout       (VkPipelineLayout *layout, 
                                                 const Device device, 
                                                 const VkDescriptorSetLayout *descriptorSetLayout);
VkPipelineLayout     getLayout                  (GraphicsPipeline pipeline);
VkDescriptorSetArray getPipelineDescriptorSets  (GraphicsPipeline pipeline);
BBError              createPipelineConfig       (PipelineConfig *config,
                                                 const SwapChain swapchain, 
                                                 UniformBuffer uniformBuffers[], 
                                                 const VkDescriptorSetLayout descriptorSetLayout, 
                                                 const VkDescriptorSetArray descriptorSets,
                                                 const VkPipelineLayout pipelineLayout);

#endif
