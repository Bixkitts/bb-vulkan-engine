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

        //Vk objects that need to be properly destroyed!
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        
        uint32_t subpass = 0; // There are subpasses created and 
                              // configured in the RenderPass
    };
    struct GraphicsPipeline
    {
        Device* device;
        SwapChain* swapchain;
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
            SwapChain* swapchain,
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            PipelineConfig* configInfo);

    void destroyPipeline(GraphicsPipeline* pipeline);

    static void cleanupShaderModules(GraphicsPipeline* pipeline);

    VkPipelineLayout createPipelineLayout(Device *device, VkDescriptorSetLayout *descriptorSetLayout);
//Descriptor set stuff
    VkDescriptorSetLayout createDescriptorSetLayout(Device *device);
    
//Make shader Modules
    static void createVertShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
    static void createFragShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
}


#endif
