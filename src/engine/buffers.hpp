#ifndef BVE_BUFFER_VERTEX
#define BVE_BUFFER_VERTEX

#include "defines.hpp"
#include <vulkan/vulkan_core.h>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vertex.hpp"
#include "model.hpp"
#include "device.hpp"

typedef struct VulkanBuffer_T VulkanBuffer_T;
typedef VulkanBuffer_T StagingBuffer_T;
typedef VulkanBuffer_T VertexBuffer_T;
typedef VulkanBuffer_T IndexBuffer_T;
typedef VulkanBuffer_T UniformBuffer_T;

BB_OPAQUE_HANDLE(VulkanBuffer);
BB_OPAQUE_HANDLE(StagingBuffer);
BB_OPAQUE_HANDLE(VertexBuffer);
BB_OPAQUE_HANDLE(IndexBuffer);
BB_OPAQUE_HANDLE(UniformBuffer);

//TODO: GLM shit
//3d transformation matrices for uniform buffers, should mb get moved to it's own area
typedef struct PerObjectMatrices
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
}PerObjectMatrices;
//-------------------------------------------------------------------------------


BBError         createVertexBuffer       (VertexBuffer *vBuffer, 
                                          const Device device, 
                                          Model model);
BBError         createIndexBuffer        (IndexBuffer *iBuffer, 
                                          const Device device, 
                                          Model model);
BBError         createUniformBuffer      (UniformBuffer *uBuffer, 
                                          const Device device, 
                                          const size_t contentsSize);
void            destroyBuffer            (VulkanBuffer *v);

// These functions should maybe be bundled in with drawing
void            drawVertexBuffer         (VertexBuffer vertexBuffer, 
                                          VkCommandBuffer commandBuffer);
void            bindVertexBuffer         (VertexBuffer vertexBuffer, 
                                          VkCommandBuffer commandBuffer);
void            drawIndexBuffer          (IndexBuffer indexBuffer, 
                                          VkCommandBuffer commandBuffer);
void            bindIndexBuffer          (IndexBuffer indexBuffer, 
                                          VkCommandBuffer commandBuffer);
VkBuffer        getVkBuffer              (VulkanBuffer buffer);

// big main buffer creation function
BBError         createBuffer             (const VkDeviceSize size,
                                          const VkBufferUsageFlags usage,
                                          const VkMemoryPropertyFlags properties,
                                          VkBuffer buffer,
                                          VkDeviceMemory bufferMemory,
                                          const Device theGPU);

VkCommandBuffer beginSingleTimeCommands  (Device theGPU);
void            endSingleTimeCommands    (VkCommandBuffer *commandBuffer, 
                                          Device theGPU);

void            copyBuffer               (const Device device, 
                                          const VkBuffer srcBuffer, 
                                          VkBuffer dstBuffer, 
                                          const VkDeviceSize size);
void            copyBufferToImage        (Device device,
                                          VkBuffer buffer, 
                                          VkImage image, 
                                          uint32_t width, 
                                          uint32_t height, 
                                          uint32_t layerCount);
//----------------------------------------------------
//Allocation Stuff, maybe moved to it's own files
//
VkDeviceMemory  allocateDeviceMemory     (Device device, 
                                          VkBuffer buffer,
                                          VkMemoryPropertyFlags properties, 
                                          VkDeviceSize size);
//----------------------------------------------------
void            createImageWithInfo      (const VkImageCreateInfo *imageInfo,
                                          const VkMemoryPropertyFlags properties,
                                          VkImage *image,
                                          VkDeviceMemory *imageMemory,
                                          Device theGPU);
// ------------------------------------
#endif
