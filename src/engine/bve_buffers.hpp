#ifndef BVE_BUFFER_VERTEX
#define BVE_BUFFER_VERTEX

#include <vulkan/vulkan_core.h>
#include "bve_vertex.hpp"
#include "bve_model.hpp"
#include <iostream>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
namespace bve
{
struct VertexBuffer
{
    Device *device;
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
    uint32_t vertexCount;
};

typedef VertexBuffer StagingBuffer;

VertexBuffer *createVertexBuffer(Device *device, const uint32_t vertexCount);
void destroyBuffer(VertexBuffer *v);

void drawVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer);
void bindVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer);


void copyToDevice(VertexBuffer *v, Model *model);
}
#endif
