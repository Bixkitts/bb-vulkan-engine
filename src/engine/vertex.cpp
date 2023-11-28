#include "vertex.hpp"

// How the vertex buffer itself is bound and interpreted
void getVertexInputBindingDescriptions (VertexInputBindingDescriptions *bindingDescriptions)
{
    bindingDescriptions[0].data.binding   = 0;
    bindingDescriptions[0].data.stride    = sizeof(Vertex);
    bindingDescriptions[0].data.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

// This describes how the data is laid out PER VERTEX.
// Currently, it's a position coordinate followed by a texture UV!
void getVertexInputAttributeDescriptions (VertexInputAttributeDescriptions *attributeDescriptions)
{
    attributeDescriptions[0].data.binding = 0;
    attributeDescriptions[0].data.location = 0;
    attributeDescriptions[0].data.format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].data.offset = 0;

    attributeDescriptions[1].data.binding = 0;
    attributeDescriptions[1].data.location = 1;
    attributeDescriptions[1].data.format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].data.offset = offsetof(Vertex, texCoord);
}
