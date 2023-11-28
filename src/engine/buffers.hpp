#ifndef BVE_BUFFER_VERTEX
#define BVE_BUFFER_VERTEX

#include <vulkan/vulkan_core.h>
#include <iostream>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vertex.hpp"
#include "swap_chain.hpp"
#include "model.hpp"
#include "error_handling.h"

//TODO: GLM shit
//3d transformation matrices for uniform buffers, should mb get moved to it's own area
typedef struct PerObjectMatrices
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
}PerObjectMatrices;
//-------------------------------------------------------------------------------
typedef struct VulkanBuffer_S
{
    Device         device;
    VkBuffer       buffer;
    VkDeviceMemory deviceMemory;
    void          *mapped;       //pointer to mapped buffer if that is the case
    uint32_t       size;
}VulkanBuffer_T, *VulkanBuffer;


typedef VulkanBuffer_T StagingBuffer_T, *StagingBuffers;
typedef VulkanBuffer_T VertexBuffer_T,  *VertexBuffers;
typedef VulkanBuffer_T IndexBuffer_T,   *IndexBuffers;
typedef VulkanBuffer_T UniformBuffer_T, *UniformBuffers;

BBError         createVertexBuffer      (VertexBuffers vBuffer, 
                                         const Device device, 
                                         Model *model);
BBError         createIndexBuffer       (IndexBuffers iBuffer, 
                                         const Device device, 
                                         Model *model);
BBError         createUniformBuffer     (UniformBuffers uBuffer, 
                                         const Device device, 
                                         const size_t contentsSize);

//TODO:
//Don't need these yet, each entity should typically only need
//one buffer each
//std::vector<VertexBuffer*> createVertexBuffers(Device device, std::vector<Model*> models);
//std::vector<IndexBuffer*> createIndexBuffers(Device device, std::vector<Model*> models);
BBError         createUniformBuffers    (UniformBuffers uBuffer, 
                                         const Device device, 
                                         const size_t contentsSize);

void            destroyBuffer           (VulkanBuffer v);

// These functions should maybe be bundled in with drawing
void            drawVertexBuffer        (VertexBuffers vertexBuffer, 
                                         VkCommandBuffer commandBuffer);
void            bindVertexBuffer        (VertexBuffers vertexBuffer, 
                                         VkCommandBuffer commandBuffer);
void            drawIndexBuffer         (IndexBuffers indexBuffer, 
                                         VkCommandBuffer commandBuffer);
void            bindIndexBuffer         (IndexBuffers indexBuffer, 
                                         VkCommandBuffer commandBuffer);

// big main buffer creation function
BBError         createBuffer            (const VkDeviceSize size,
                                         const VkBufferUsageFlags usage,
                                         const VkMemoryPropertyFlags properties,
                                         VkBuffer buffer,
                                         VkDeviceMemory bufferMemory,
                                         const Device theGPU);

VkCommandBuffer beginSingleTimeCommands (Device theGPU);
void            endSingleTimeCommands   (VkCommandBuffer commandBuffer, 
                                         Device theGPU);

void            copyBuffer              (const Device theGPU, 
                                         const VkBuffer srcBuffer, 
                                         VkBuffer dstBuffer, 
                                         const VkDeviceSize size);
void            copyBufferToImage       (Device theGPU,
                                         VkBuffer buffer, 
                                         VkImage image, 
                                         uint32_t width, 
                                         uint32_t height, 
                                         uint32_t layerCount);
//----------------------------------------------------
//Allocation Stuff, maybe moved to it's own files
//
VkDeviceMemory  allocateDeviceMemory    (Device theGPU, 
                                         VkBuffer buffer,
                                         VkMemoryPropertyFlags properties, 
                                         VkDeviceSize size);
//----------------------------------------------------
void            createImageWithInfo     (const VkImageCreateInfo imageInfo,
                                         const VkMemoryPropertyFlags properties,
                                         VkImage image,
                                         VkDeviceMemory imageMemory,
                                         Device theGPU);
// ------------------------------------
void            copyVertsToDeviceMem    (StagingBuffers sb, 
                                         Vertex *vertices, 
                                         uint32_t vertexCount);
void            copyIndecesToDeviceMem  (StagingBuffers sb, 
                                         uint32_t *indeces, 
                                         uint32_t indexCount);
#endif
