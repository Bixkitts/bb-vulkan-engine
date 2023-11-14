#ifndef BVE_VERTEX
#define BVE_VERTEX

#include <vector>

#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

struct Vertex
{
    // TODO:
    // GLM stuff, remove :/
    glm::vec2 position;
    glm::vec2 texCoord;
};
std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

#endif
