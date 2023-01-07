#ifndef BVE_MODEL
#define BVE_MODEL
#include <vulkan/vulkan_core.h>
#include "bve_vertex.hpp"

#include "bve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace bve
{
    typedef std::vector<Vertex> Model;
    
    std::vector<Model*> loadModels(Device *device);


    Model *createModel(Device *device, std::vector<Vertex> &vertices); 
    void bindModel(Model *model, VkCommandBuffer commandBuffer);
    void drawModel(Model *model, VkCommandBuffer commandBuffer);
}

#endif
