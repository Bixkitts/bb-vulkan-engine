#ifndef BVE_VERTEX
#define BVE_VERTEX

#include <vulkan/vulkan_core.h>
#include <vector>

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
    std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
}
#endif
