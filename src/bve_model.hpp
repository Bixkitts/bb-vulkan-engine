#ifndef BVE_MODEL
#define BVE_MODEL
#include "bve_device.hpp"
#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace bve
{
    struct Vertex
    {
        glm::vec2 position;

        static std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        static std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    };

    struct BveModel
    {
        BveDevice *device;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexMemory;
        uint32_t vertexCount;
    };

    BveModel *createBveModel(BveDevice *device, std::vector<Vertex> &vertices); 
    void createVertexBuffers(BveModel *model, const std::vector<Vertex> &vertices);

    std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();


    void bindModel(BveModel *model, VkCommandBuffer commandBuffer);
    void drawModel(BveModel *model, VkCommandBuffer commandBuffer);

    std::vector<BveModel*> loadModels(BveDevice *device);

}

#endif
