#include "buffers.hpp"
#include "error_handling.h"
#include "swap_chain.hpp"
#include "config_buffers.hpp"
#include <vulkan/vulkan_core.h>

BBError createVertexBuffer(VertexBuffer *vBuffer, Device *device, Model *model)
{
    // NOTE - maybe ZERO this
    StagingBuffer sbuffer;
    sbuffer.device = device;
    // TODO:
    // assert(model->vertexCount >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(Vertex) * model->vertexCount;
    createBuffer(bufferSize,
           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
           sbuffer.buffer,
           sbuffer.deviceMemory,
           sbuffer.device);

    copyVertsToDeviceMem(&sbuffer, model->vertices, model->vertexCount);
    
    vBuffer = (VertexBuffer*)calloc(1, sizeof(VertexBuffer));
    vBuffer->device = device;
    vBuffer->size = model->vertexCount;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vBuffer->buffer, vBuffer->deviceMemory, vBuffer->device);

    copyBuffer(device, sbuffer.buffer, vBuffer->buffer, bufferSize);
   
    destroyBuffer(&sbuffer); 

    return BB_ERROR_OK;
}

IndexBuffer *createIndexBuffer(Device *device, Model *model)
{
    StagingBuffer sbuffer = {};
    sbuffer.device = device;
    //TODO:
    //assert(model->indeces.size() >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(model->indeces[0]) * model->indexCount;
    createBuffer(bufferSize,
           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
           sbuffer.buffer,
           sbuffer.deviceMemory,
           sbuffer.device);

    copyIndecesToDeviceMem(&sbuffer, model->indeces, model->indexCount);
    
    auto *ibuffer = new IndexBuffer{};
    ibuffer->device = device;
    ibuffer->size = model->indexCount;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibuffer->buffer, ibuffer->deviceMemory, ibuffer->device);

    copyBuffer(device, sbuffer.buffer, ibuffer->buffer, bufferSize);
   
    destroyBuffer(&sbuffer); 

    return ibuffer;
}

BBError createUniformBuffer(UniformBuffer *uBuffer, Device *device, size_t contentsSize)
{
    uBuffer->device = device;
    uBuffer->size = 1;  //a uniform buffer is going to typically contain a single struct
    VkDeviceSize bufferSize = contentsSize;
    createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uBuffer->buffer, uBuffer->deviceMemory, device); 

    vkMapMemory(device->logical, uBuffer->deviceMemory, 0, bufferSize, 0, &uBuffer->mapped);

    return BB_ERROR_OK;
}

// we don't need a different vertex buffer for each frame,
// I'll leave this dormant for now.
BBError createVertexBuffers(VertexBuffer *vBuffer, Device *device, Model *model)
{
//    std::vector<VertexBuffer*> vBuffers;
//    for(int i = 0; i < models.size(); i++)
//    {
//        vBuffers.push_back(
//        createVertexBuffer(device, models[i])
//        );
//    } 
//
//    return vBuffers;
    return BB_ERROR_UNKNOWN;
}

// TODO:
// just like above I don't need multiple index buffers per entity
BBError createIndexBuffers(Device *device, std::vector<Model*> models)
{
//    std::vector<IndexBuffer*> iBuffers;
//    for(int i = 0; i < models.size(); i++)
//    {
//        iBuffers.push_back(
//        createIndexBuffer(device, models[i])
//        );
//    } 
//
    return BB_ERROR_UNKNOWN;
}
BBError createUniformBuffers(UniformBuffer *uBuffer, Device *device, size_t contentsSize)
{
    //TODO: MALLOC, NO FREE
    uBuffer = (UniformBuffer*)calloc(MAX_FRAMES_IN_FLIGHT, sizeof(UniformBuffer));
    if (uBuffer == NULL)
    {
        return BB_ERROR_MEM;
    }
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        createUniformBuffer(&uBuffer[i], device, contentsSize);
    } 

    return BB_ERROR_OK;
}
void destroyBuffer(VulkanBuffer *v)
{
    vkDestroyBuffer(v->device->logical, v->buffer, nullptr);
    vkFreeMemory(v->device->logical, v->deviceMemory, nullptr);
    //TODO: make this FREE() when you're sure all the buffers are
    //good to go.
    delete v;
}

// Use drawIndexBuffer() instead
void drawVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer)
{
    vkCmdDraw(commandBuffer, vertexBuffer->size, 1, 0, 0);
}
// In this function I should consider binding multiple vertex buffers
// in one vulkan call
void bindVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer commandBuffer)
{
    //TODO:
    //Not 100% sure this should be an array
    VkBuffer buffers[] = {vertexBuffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0 ,1, buffers, offsets); 
}

void drawIndexBuffer(IndexBuffer *indexBuffer, VkCommandBuffer commandBuffer)
{
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexBuffer->size), 1, 0, 0, 0);
}
void bindIndexBuffer(IndexBuffer *indexBuffer, VkCommandBuffer commandBuffer)
{
    VkBuffer buffer = {indexBuffer->buffer};
    VkDeviceSize offset = {0};
    vkCmdBindIndexBuffer(commandBuffer, buffer, offset, VK_INDEX_TYPE_UINT32); 
}

void copyVertsToDeviceMem(StagingBuffer *sb, Vertex *vertices, uint32_t vertexCount)
{
    uint32_t size = vertexCount * sizeof(Vertex);
    void *data;
    vkMapMemory(sb->device->logical, sb->deviceMemory, 0, size, 0, &data);
    // TODO:
    // beware of this cast
    memcpy(data, vertices, (size_t)size);
    vkUnmapMemory(sb->device->logical, sb->deviceMemory);
}

void copyIndecesToDeviceMem(StagingBuffer *sb, std::vector<uint32_t> &indeces)
{
    uint32_t size = indeces.size() * sizeof(indeces[0]);
    void *data;
    vkMapMemory(sb->device->logical, sb->deviceMemory, 0, size, 0, &data);
    memcpy(data, indeces.data(), static_cast<size_t>(size));
    vkUnmapMemory(sb->device->logical, sb->deviceMemory);
}

//TODO: manage device memory allocation instead of allocating for every new buffer
void createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer buffer,
    VkDeviceMemory bufferMemory,
    Device* theGPU) 
{
    VkBufferCreateInfo bufferInfo = bufferCreateInfo(size, usage);

    if (vkCreateBuffer(theGPU->logical, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    // TODO: device memory allocation needs to be separated
    bufferMemory = allocateDeviceMemory(theGPU, buffer, properties, size);

    vkBindBufferMemory(theGPU->logical, buffer, bufferMemory, 0);
}

//TODO:
//Perhaps an OUT parameter here instead of return.
VkDeviceMemory allocateDeviceMemory(Device *theGPU, VkBuffer buffer,VkMemoryPropertyFlags properties, VkDeviceSize size)
{
    VkDeviceMemory deviceMemory = {};
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(theGPU->logical, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = memoryAllocateInfo(memRequirements, properties, theGPU);

    if (vkAllocateMemory(theGPU->logical, &allocInfo, nullptr, &deviceMemory) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }
    return deviceMemory;
}

void copyBuffer(Device* theGPU, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(theGPU);
  
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
  
    endSingleTimeCommands(commandBuffer, theGPU);
}

void copyBufferToImage(
        Device* theGPU,
    VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(theGPU);
  
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
  
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;
  
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};
  
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
    endSingleTimeCommands(commandBuffer, theGPU);
}

void createImageWithInfo(
    const VkImageCreateInfo &imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &imageMemory,
      Device* theGPU) 
{
    if (vkCreateImage(theGPU->logical, &imageInfo, nullptr, &image) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(theGPU->logical, image, &memRequirements);
  
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, theGPU);

    if (vkAllocateMemory(theGPU->logical, &allocInfo, nullptr, &imageMemory) 
            != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate image memory!");
    }
  
    if (vkBindImageMemory(theGPU->logical, image, imageMemory, 0) 
            != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to bind image memory!");
    }
}
