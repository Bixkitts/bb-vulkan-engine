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
struct VulkanBuffer
{
    Device *device;
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
    uint32_t vertexCount;
};

typedef VulkanBuffer StagingBuffer;
typedef VulkanBuffer VertexBuffer;

VertexBuffer *createVertexBuffer(Device *device, const uint32_t vertexCount);
void destroyBuffer(VertexBuffer *v);

void drawVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer);
void bindVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer);

// copy these
void createDeviceBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer &buffer,
    VkDeviceMemory &bufferMemory,
    Device *theGPU);

VkCommandBuffer beginSingleTimeCommands(Device *theGPU);
void endSingleTimeCommands(VkCommandBuffer commandBuffer, Device *theGPU);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void copyBufferToImage(
        VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

void createImageWithInfo(
    const VkImageCreateInfo &imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &imageMemory,
    Device *theGPU);
// ------------------------------------
void copyToDevice(VertexBuffer *v, Model *model);
}
#endif
