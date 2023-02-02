#ifndef BVE_PIPELINE
#define BVE_PIPELINE

#include "buffers.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

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
        std::vector<VkDescriptorSet> descriptorSets;
        VkDescriptorPool descriptorPool;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;

        std::vector<UniformBuffer*> uniformBuffers; //pointer to the main vector of uniform Buffers.
        
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
    
    PipelineConfig* defaultPipelineConfigInfo(SwapChain* swapchain, std::vector<UniformBuffer*> &uniformBuffers);

    GraphicsPipeline* createGraphicsPipeline(
            Device* device,
            SwapChain* swapchain,
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            PipelineConfig* configInfo);

    void destroyPipeline(GraphicsPipeline* pipeline);

    static void cleanupShaderModules(GraphicsPipeline* pipeline);

    VkPipelineLayout createPipelineLayout(Device *device, bve::PipelineConfig *config);
//Descriptor set stuff
    VkDescriptorSetLayout createDescriptorSetLayout(Device *device);
    VkDescriptorPool createDescriptorPool(Device *device);
    std::vector<VkDescriptorSet> createDescriptorSets(Device *device, PipelineConfig *config);
    
//Make shader Modules
    static void createVertShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
    static void createFragShaderModule(GraphicsPipeline *pipeline, const std::vector<char>& code);
}


#endif
