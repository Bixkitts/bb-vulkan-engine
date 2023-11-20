#ifndef BVE_BUFFER_VERTEX
#define BVE_BUFFER_VERTEX

#include <vulkan/vulkan_core.h>
#include <iostream>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vertex.hpp"
#include "model.hpp"
#include "error_handling.h"

//TODO: GLM shit
//3d transformation matrices for uniform buffers, should mb get moved to it's own area
typedef struct PerObjectMatrices
{
    glm::mat4 model;
}PerObjectMatrices;
typedef struct ViewMatrices
{
    glm::mat4 view;
    glm::mat4 proj;
}ViewMatrices;
//-------------------------------------------------------------------------------
typedef struct VulkanBuffer
{
    Device *device;
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
    void *mapped;       //pointer to mapped buffer if that is the case
    uint32_t size;
}VulkanBuffer;


typedef VulkanBuffer StagingBuffer;
typedef VulkanBuffer VertexBuffer;
typedef VulkanBuffer IndexBuffer;
typedef VulkanBuffer UniformBuffer;

BBError createVertexBuffer(VertexBuffer *vBuffer, Device *device, Model *model);
BBError createIndexBuffer(IndexBuffer *iBuffer, Device *device, Model *model);
BBError createUniformBuffer(UniformBuffer *uBuffer, Device *device, size_t contentsSize);

//TODO:
//Don't need these yet, each entity should typically only need
//one buffer each
//std::vector<VertexBuffer*> createVertexBuffers(Device *device, std::vector<Model*> models);
//std::vector<IndexBuffer*> createIndexBuffers(Device *device, std::vector<Model*> models);
BBError createUniformBuffers(UniformBuffer *uBuffer, Device *device, size_t contentsSize);

void destroyBuffer(VulkanBuffer *v);

// These functions should maybe be bundled in with drawing
void drawVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer commandBuffer);
void bindVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer commandBuffer);
void drawIndexBuffer(IndexBuffer *indexBuffer, VkCommandBuffer commandBuffer);
void bindIndexBuffer(IndexBuffer *indexBuffer, VkCommandBuffer commandBuffer);

// big main buffer creation function
void createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer buffer,
    VkDeviceMemory bufferMemory,
    Device *theGPU);

VkCommandBuffer beginSingleTimeCommands(Device *theGPU);
void endSingleTimeCommands(VkCommandBuffer commandBuffer, Device *theGPU);

void copyBuffer(Device *theGPU, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void copyBufferToImage( Device *theGPU,
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
void copyVertsToDeviceMem(StagingBuffer *sb, Vertex *vertices, uint32_t vertexCount);
void copyIndecesToDeviceMem(StagingBuffer *sb, uint32_t *indeces, uint32_t indexCount);
#endif
