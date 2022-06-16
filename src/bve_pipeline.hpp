#ifndef BVE_PIPELINE
#define BVE_PIPELINE

#include "bve_device.hpp"

#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>

namespace bve
{
    struct PipelineConfigInfo
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
    struct BveGraphicsPipeline
    {
        BveDevice* myPipelineDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };

    void bindPipeline(BveGraphicsPipeline *pipeline, VkCommandBuffer comandBuffer);

    std::vector<char> readFile(const std::string& filepath);
    
    PipelineConfigInfo* defaultPipelineConfigInfo(uint32_t width, uint32_t height);

    BveGraphicsPipeline *createBveGraphicsPipeline(
            BveDevice* device,
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            PipelineConfigInfo* configInfo);

    void destroyBveGraphicsPipeline(BveGraphicsPipeline *pipeline);

    VkPipelineLayout createPipelineLayout(BveDevice *device);
    void createCommanBuffers();


    void createShaderModule(BveDevice *device, const std::vector<char>& code, VkShaderModule* shaderModule);

}


#endif
