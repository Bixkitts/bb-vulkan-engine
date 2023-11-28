#include "vertex.hpp"

// How the vertex buffer itself is bound and interpreted
void getVertexInputBindingDescriptions (VertexInputBindingDescriptions *bindingDescriptions)
{
    bindingDescriptions->data[0].binding   = 0;
    bindingDescriptions->data[0].stride    = sizeof(Vertex);
    bindingDescriptions->data[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    bindingDescriptions->count = BB_VERTEX_INPUT_BINDING_DESC_COUNT;
}

// This describes how the data is laid out PER VERTEX.
// Currently, it's a position coordinate followed by a texture UV!
void getVertexInputAttributeDescriptions (VertexInputAttributeDescriptions *attributeDescriptions)
{
    attributeDescriptions->data[0].binding  = 0;
    attributeDescriptions->data[0].location = 0;
    attributeDescriptions->data[0].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions->data[0].offset   = 0;

    attributeDescriptions->data[1].binding  = 0;
    attributeDescriptions->data[1].location = 1;
    attributeDescriptions->data[1].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions->data[1].offset   = offsetof(Vertex, texCoord);

    attributeDescriptions->count = BB_VERTEX_INPUT_ATTRIBUTE_DESC_COUNT;
}
