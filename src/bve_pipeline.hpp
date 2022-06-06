#ifndef BVE_PIPELINE
#define BVE_PIPELINE

#include "bve_device.hpp"

#include <vector>
#include <string>

namespace bve
{
    struct PipelineConfigInfo
    {
        
    };
    struct graphicsPipeline
    {
        BveDevice* myPipelineDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };

    std::vector<char> readFile(const std::string& filepath);

    void createGraphicsPipeline(
            BveDevice* device,
            const std::string& vertFilepath, 
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);

    PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

    void createShaderModule(BveDevice *device, const std::vector<char> code, VkShaderModule* shaderModule);

}


#endif
