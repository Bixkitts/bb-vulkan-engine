#include "bve_buffers.hpp"
#include "config_buffers.hpp"
#include <vulkan/vulkan_core.h>
namespace bve
{

VertexBuffer *createVertexBuffer(Device *device, Model *model)
{
    auto *sbuffer = new StagingBuffer{};
    sbuffer->device = device;
    assert(model->size() >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(Vertex) * model->size();
    createDeviceBuffer(bufferSize,
           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
           sbuffer->buffer,
           sbuffer->deviceMemory,
           sbuffer->device);

    copyToDeviceMem(sbuffer, model);
    
    auto *vbuffer = new VertexBuffer{};
    vbuffer->device = device;
    vbuffer->vertexCount = model->size();
    createDeviceBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbuffer->buffer, vbuffer->deviceMemory, vbuffer->device);

    copyBuffer(device, sbuffer->buffer, vbuffer->buffer, bufferSize);
    
    vkDestroyBuffer(device->logical, sbuffer->buffer, nullptr);
    vkFreeMemory(device->logical, sbuffer->deviceMemory, nullptr);
    delete sbuffer;
    return vbuffer;
}
std::vector<VertexBuffer*> createVertexBuffers(Device *device, std::vector<Model*> models)
{
    std::vector<VertexBuffer*> vBuffers;
    for(int i = 0; i < models.size(); i++)
    {
        vBuffers.push_back(
        createVertexBuffer(device, models[i])
        );
    } 

    return vBuffers;
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


void copyToDeviceMem(StagingBuffer *sb, Model *model)
{
    uint32_t size = model->size() * sizeof(Vertex);
    void *data;
    vkMapMemory(sb->device->logical, sb->deviceMemory, 0, size, 0, &data);
    memcpy(data, model->data(), static_cast<size_t>(size));
    vkUnmapMemory(sb->device->logical, sb->deviceMemory);
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

    bufferMemory = allocateDeviceMemory(theGPU, buffer, properties, size);

    vkBindBufferMemory(theGPU->logical, buffer, bufferMemory, 0);
}

VkDeviceMemory allocateDeviceMemory(Device *theGPU, VkBuffer buffer,VkMemoryPropertyFlags properties, VkDeviceSize size)
{
    VkDeviceMemory deviceMemory = {};
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(theGPU->logical, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = config::memoryAllocateInfo(memRequirements, properties, theGPU);

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
}
