#ifndef BVE_PIPELINE
#define BVE_PIPELINE

#include "bve_device.hpp"
#include "bve_swap_chain.hpp"

#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>

namespace bve
{
    struct PipelineConfig
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        
        //These two have VkDestroy functions that need to
        //be called. They are here in the config struct
        //because they get passed in to vkCreateGraphicsPipelines()
        //with the rest of the above data after being translated
        //to a VkPipelineLayoutCreateInfo
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;

        uint32_t subpass = 0;
    };
    struct GraphicsPipeline
    {
        Device* device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
        PipelineConfig *pipelineConfig;
    };

    void bindPipeline(GraphicsPipeline* pipeline, VkCommandBuffer commandBuffer);
    
    //move this function to an IO cpp file
    std::vector<char> readFile(const std::string& filepath);
    
    PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain);

    GraphicsPipeline* createGraphicsPipeline(
            Device* device,
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            PipelineConfig* configInfo);

    void destroyPipeline(GraphicsPipeline* pipeline);

    static void cleanupShaderModules(GraphicsPipeline* pipeline);

    static void createPipelineLayout(GraphicsPipeline *pipeline);
    static void createRenderPass(GraphicsPipeline *pipeline);

    static void createVertShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
    static void createFragShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
}


#endif
