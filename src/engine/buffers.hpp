#ifndef BVE_BUFFER_VERTEX
#define BVE_BUFFER_VERTEX

#include <vulkan/vulkan_core.h>
#include "vertex.hpp"
#include "model.hpp"
#include <iostream>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
namespace bve
{
//3d transformation matrices for uniform buffer, should mb get moved to it's own area
struct Matrices
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
//-------------------------------------------------------------------------------
struct VulkanBuffer
{
    Device *device;
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
    uint32_t size;
    void *mapped = nullptr;       //pointer to mapped buffer if that is the case
};


typedef VulkanBuffer StagingBuffer;
typedef VulkanBuffer VertexBuffer;
typedef VulkanBuffer IndexBuffer;
typedef VulkanBuffer UniformBuffer;

VertexBuffer *createVertexBuffer(Device *device, Model *model);
IndexBuffer *createIndexBuffer(Device *device, Model *model);
UniformBuffer *createUniformBuffer(Device *device, size_t contentsSize);

std::vector<VertexBuffer*> createVertexBuffers(Device *device, std::vector<Model*> models);
std::vector<IndexBuffer*> createIndexBuffers(Device *device, std::vector<Model*> models);
std::vector<UniformBuffer*> createUniformBuffers(Device *device, size_t contentsSize);

void destroyBuffer(VulkanBuffer *v);

void drawVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer);
void bindVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer);
void drawIndexBuffer(IndexBuffer *indexBuffer, VkCommandBuffer &commandBuffer);
void bindIndexBuffer(IndexBuffer *indexBuffer, VkCommandBuffer &commandBuffer);

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

void copyBuffer(Device *theGPU, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void copyBufferToImage(
        VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
//----------------------------------------------------
//Allocation Stuff, maybe moved to it's own files
//
VkDeviceMemory allocateDeviceMemory(Device *theGPU, VkBuffer buffer,VkMemoryPropertyFlags properties, VkDeviceSize size);
//----------------------------------------------------
void createImageWithInfo(
    const VkImageCreateInfo &imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &imageMemory,
    Device *theGPU);
// ------------------------------------
void copyToDeviceMem(StagingBuffer *sb, std::vector<Vertex> &vertices);
void copyToDeviceMem(StagingBuffer *sb, std::vector<uint32_t> &indeces);
}
#endif
