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

        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;

        uint32_t subpass = 0;
    };
    struct GraphicsPipeline
    {
        Device* myPipelineDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };

    void bindPipeline(GraphicsPipeline* pipeline, VkCommandBuffer comandBuffer);

    std::vector<char> readFile(const std::string& filepath);
    
    PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain);

    GraphicsPipeline* createGraphicsPipeline(
            Device* device,
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            PipelineConfig* configInfo);

    void destroyBveGraphicsPipeline(GraphicsPipeline* pipeline);

    VkPipelineLayout createPipelineLayout(Device* device);

    void createCommanBuffers();


    VkShaderModule createShaderModule(Device* device, const std::vector<char>& code);

}


#endif
