#ifndef BVE_VERTEX
#define BVE_VERTEX

#include <vector>

#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define BB_VERTEX_INPUT_BINDING_DESC_COUNT 1
#define BB_VERTEX_INPUT_ATTRIBUTE_DESC_COUNT 2

typedef struct Vertex
{
    // TODO:
    // GLM stuff, remove :/
    glm::vec2 position;
    glm::vec2 texCoord;
}Vertex;

typedef struct VertexInputBindingDescriptions
{
    VkVertexInputBindingDescription data[BB_VERTEX_INPUT_BINDING_DESC_COUNT];
    uint32_t                        count;
}VertexInputBindingDescriptions;

typedef struct VertexInputAttributeDescriptions
{
    VkVertexInputAttributeDescription data[BB_VERTEX_INPUT_ATTRIBUTE_DESC_COUNT];
    uint32_t                          count;
}VertexInputAttributeDescriptions;

void getVertexInputBindingDescriptions   (VertexInputBindingDescriptions *bindingDescriptions);
void getVertexInputAttributeDescriptions (VertexInputAttributeDescriptions *attributeDescriptions);

#endif
