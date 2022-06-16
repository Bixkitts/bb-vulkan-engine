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

        static std::vector<VkVertexInputBindingDescription> bindingDescrptions;
        static std::vector<VkVertexInputBindingDescription> attributeDescriptions;

    };

    struct BveModel
    {
        BveDevice *device;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexMemory;
        uint32_t vertexCount;
    };

    BveModel *createBveModel(BveDevice *device); 
    void createVertexBuffers(const std::vector<Vertex> &vertices);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

}

#endif
