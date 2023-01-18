#include "bve_buffers.hpp"
#include "config_buffers.hpp"
namespace bve
{

VertexBuffer *createVertexBuffer(Device *device, const uint32_t vertexCount)
{
    VertexBuffer *v = new VertexBuffer{};
    v->device = device;
    v->vertexCount = vertexCount;
    assert(v->vertexCount >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(Vertex) * v->vertexCount;
    createDeviceBuffer(bufferSize,
           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
           v->buffer,
           v->deviceMemory,
           v->device);

    return v;
}

void destroyBuffer(VertexBuffer *v)
{
    vkDestroyBuffer(v->device->logical, v->buffer, nullptr);
    vkFreeMemory(v->device->logical, v->deviceMemory, nullptr);
}

void drawVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer)
{
    vkCmdDraw(commandBuffer, vertexBuffer->vertexCount, 1, 0, 0);
}

void bindVertexBuffer(VertexBuffer *vertexBuffer, VkCommandBuffer &commandBuffer)
{
    VkBuffer buffers[] = {vertexBuffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0 ,1, buffers, offsets); 
}

//--------------------------------------------------------
//
//functions to copy to the device
//--------------------------------------------------------

void copyToDevice(VertexBuffer *v, Model *model)
{
    uint32_t size = v->vertexCount * sizeof(Vertex);
    void *data;
    vkMapMemory(v->device->logical, v->deviceMemory, 0, size, 0, &data);
    memcpy(data, model->data(), static_cast<size_t>(size));
    vkUnmapMemory(v->device->logical, v->deviceMemory);
}

void createDeviceBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer &buffer,
    VkDeviceMemory &bufferMemory,
    Device* theGPU) 
{
    VkBufferCreateInfo bufferInfo = config::bufferCreateInfo(size, usage);

    if (vkCreateBuffer(theGPU->logical, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(theGPU->logical, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = config::memoryAllocateInfo(memRequirements, properties, theGPU);

    if (vkAllocateMemory(theGPU->logical, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(theGPU->logical, buffer, bufferMemory, 0);
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
}
