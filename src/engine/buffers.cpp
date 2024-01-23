#include "buffers.hpp"
#include "defines.hpp"
#include "error_handling.h"

struct VulkanBuffer_T
{
    Device         device;
    VkBuffer       buffer;
    VkDeviceMemory deviceMemory;
    void          *mapped;       //pointer to mapped buffer if that is the case
    VkDeviceSize   size;
};

static void initStagingBuffer       (StagingBuffer_T *sBuffer,
                                     const Device device,
                                     VkDeviceSize bufferSize);
static void destroyStagingBuffer    (StagingBuffer_T *s);
static void copyVertsToDeviceMem    (StagingBuffer sb, 
                                     Vertex *vertices, 
                                     uint32_t vertexCount);
static void copyIndecesToDeviceMem  (StagingBuffer sb, 
                                     uint32_t *indeces, 
                                     uint32_t indexCount);

static VkMemoryAllocateInfo memoryAllocateInfo (const VkMemoryRequirements memRequirements,
                                                const VkMemoryPropertyFlags properties, 
                                                const Device theGPU);
static VkBufferCreateInfo   bufferCreateInfo   (const VkDeviceSize size, 
                                                const VkBufferUsageFlags usage);

static void initStagingBuffer(StagingBuffer_T *sBuffer,
                              const Device device,
                              VkDeviceSize bufferSize)
{
    sBuffer->device = device;
    createBuffer (bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  sBuffer->buffer,
                  sBuffer->deviceMemory,
                  sBuffer->device);
}

static void destroyStagingBuffer(StagingBuffer_T *s)
{
    vkDestroyBuffer (getLogicalDevice(s->device), s->buffer, nullptr);
    vkFreeMemory    (getLogicalDevice(s->device), s->deviceMemory, nullptr);
}

BBError createVertexBuffer(VertexBuffer_T **vBuffer, 
                           const Device device, 
                           Model model)
{
    uint32_t          vertCount   = getModelVertexCount (model);
    Vertex           *verts       = getModelVerts       (model);
    VkDeviceSize      bufferSize  = sizeof(Vertex) * vertCount;
    StagingBuffer_T   sBuffer     = {};

    if (vertCount < 3) {
        return BB_ERROR_GPU_BUFFER_CREATE;
    }

    // TODO: less shitty allocation strategy for this
    *vBuffer = (VertexBuffer_T*)calloc(1, sizeof(VertexBuffer_T));
    if (*vBuffer == NULL) {
        return BB_ERROR_MEM;
    }

    initStagingBuffer    (&sBuffer, device, bufferSize);
    copyVertsToDeviceMem (&sBuffer, verts, vertCount);
    // TODO: MALLOC without free
    (*vBuffer)->device = device;
    (*vBuffer)->size   = vertCount;
    createBuffer         (bufferSize, 
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                          (*vBuffer)->buffer, 
                          (*vBuffer)->deviceMemory, 
                          (*vBuffer)->device);
    copyBuffer           (device, 
                          sBuffer.buffer, 
                          (*vBuffer)->buffer, 
                          bufferSize);
    destroyStagingBuffer (&sBuffer); 
    return BB_ERROR_OK;
}

BBError createIndexBuffer(IndexBuffer_T **iBuffer,
                          const Device device, 
                          Model model)
{
    VertIndex      *indeces    = getModelIndeces    (model);
    uint32_t        indexCount = getModelIndexCount (model);
    StagingBuffer_T sBuffer    = {};
    VkDeviceSize    bufferSize = sizeof(indeces[0]) * indexCount;

    *iBuffer = (VertexBuffer_T*)calloc(1, sizeof(VertexBuffer_T));
    if (*iBuffer == NULL) {
        return BB_ERROR_MEM;
    }

    initStagingBuffer      (&sBuffer, device, bufferSize);
    copyIndecesToDeviceMem (&sBuffer, 
                            indeces, 
                            indexCount);
    

    (*iBuffer)->device = device;
    (*iBuffer)->size   = indexCount;
    createBuffer           (bufferSize, 
                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                            (*iBuffer)->buffer, 
                            (*iBuffer)->deviceMemory, 
                            (*iBuffer)->device);
    copyBuffer             (device, 
                            sBuffer.buffer, 
                            (*iBuffer)->buffer, 
                            bufferSize);
    destroyStagingBuffer   (&sBuffer); 
    return BB_ERROR_OK;
}

BBError createUniformBuffer(UniformBuffer *uBuffer, 
                            const Device device, 
                            const size_t contentsSize)
{
    (*uBuffer)->device = device;
    (*uBuffer)->size   = 1;
    VkDeviceSize bufferSize = contentsSize;

    // TODO: need a better allocation strategy
    *uBuffer = (VertexBuffer_T*)calloc(1, sizeof(VertexBuffer_T));
    if (*uBuffer == NULL) {
        return BB_ERROR_MEM;
    }

    createBuffer (bufferSize, 
                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                  (*uBuffer)->buffer, 
                  (*uBuffer)->deviceMemory, 
                  device); 
    vkMapMemory  (getLogicalDevice(device), 
                  (*uBuffer)->deviceMemory, 
                  0, 
                  bufferSize, 
                  0, 
                  &(*uBuffer)->mapped);

    return BB_ERROR_OK;
}

void destroyBuffer(VulkanBuffer *v)
{
    if (*v == NULL) {
        return;
    }
    vkDestroyBuffer (getLogicalDevice((*v)->device), (*v)->buffer, nullptr);
    vkFreeMemory    (getLogicalDevice((*v)->device), (*v)->deviceMemory, nullptr);
    free            (*v);
    *v = NULL;
}

// Use drawIndexBuffer() instead
void drawVertexBuffer(VertexBuffer vertexBuffer, VkCommandBuffer commandBuffer)
{
    vkCmdDraw(commandBuffer, vertexBuffer->size, 1, 0, 0);
}
// In this function I should consider binding multiple vertex buffers
// in one vulkan call
void bindVertexBuffer(VertexBuffer vertexBuffer, VkCommandBuffer commandBuffer)
{
    //TODO:
    //Not 100% sure this should be an array
    VkBuffer     buffers[] = {vertexBuffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0 ,1, buffers, offsets); 
}

void drawIndexBuffer(IndexBuffer indexBuffer, VkCommandBuffer commandBuffer)
{
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexBuffer->size), 1, 0, 0, 0);
}
void bindIndexBuffer(IndexBuffer indexBuffer, VkCommandBuffer commandBuffer)
{
    VkBuffer     buffer = {indexBuffer->buffer};
    VkDeviceSize offset = {0};
    vkCmdBindIndexBuffer(commandBuffer, buffer, offset, VK_INDEX_TYPE_UINT32); 
}
VkBuffer getVkBuffer(VulkanBuffer buffer)
{
    return buffer->buffer;
}

static void copyVertsToDeviceMem(StagingBuffer sb, Vertex *vertices, uint32_t vertexCount)
{
    uint32_t  size = vertexCount * sizeof(Vertex);
    void     *data;
    vkMapMemory   (getLogicalDevice(sb->device), 
                   sb->deviceMemory, 
                   0, 
                   size, 
                   0, 
                   &data);
    // TODO:
    // beware of this cast
    memcpy        (data, 
                   vertices, 
                   (size_t)size);
    vkUnmapMemory (getLogicalDevice(sb->device), 
                   sb->deviceMemory);
}

static void copyIndecesToDeviceMem(StagingBuffer sb, uint32_t *indeces, uint32_t indexCount)
{
    size_t    size = indexCount * sizeof(indeces[0]);
    void     *data = NULL;
    vkMapMemory   (getLogicalDevice(sb->device), 
                   sb->deviceMemory, 
                   0, 
                   size, 
                   0, 
                   &data);
    memcpy        (data, 
                   indeces, 
                   size);
    vkUnmapMemory (getLogicalDevice(sb->device), 
                   sb->deviceMemory);
}

//TODO: manage device memory allocation instead of allocating for every new buffer
BBError createBuffer(const VkDeviceSize size,
                     const VkBufferUsageFlags usage,
                     const VkMemoryPropertyFlags properties,
                     VkBuffer buffer,
                     VkDeviceMemory bufferMemory,
                     const Device device) 
{
    VkBufferCreateInfo bufferInfo = bufferCreateInfo(size, usage);

    if (vkCreateBuffer(getLogicalDevice(device), &bufferInfo, nullptr, &buffer) 
        != VK_SUCCESS){
        fprintf(stderr, "\nfailed to create buffer on GPU");
        return BB_ERROR_GPU_BUFFER_CREATE;
    }

    // TODO: device memory allocation needs to be separated
    bufferMemory = allocateDeviceMemory(device, buffer, properties, size);

    vkBindBufferMemory(getLogicalDevice(device), buffer, bufferMemory, 0);
    return BB_ERROR_OK;
}

//TODO:
//Perhaps an OUT parameter here instead of return.
VkDeviceMemory allocateDeviceMemory(Device device, 
                                    VkBuffer buffer,
                                    VkMemoryPropertyFlags properties, 
                                    VkDeviceSize size)
{
    VkDeviceMemory       deviceMemory    = {};
    VkMemoryRequirements memRequirements = {};
    VkMemoryAllocateInfo allocInfo       = {};
    vkGetBufferMemoryRequirements (getLogicalDevice(device), buffer, &memRequirements);
    allocInfo = 
    memoryAllocateInfo            (memRequirements, properties, device);
    // TODO: stdlib shit
    if (
    vkAllocateMemory              (getLogicalDevice(device), &allocInfo, nullptr, &deviceMemory) 
    != VK_SUCCESS) {
        exit(1);
    }
    return deviceMemory;
}

void copyBuffer(const Device device, 
                const VkBuffer srcBuffer, 
                VkBuffer dstBuffer, 
                const VkDeviceSize size) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device);
  
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset    = 0;  // Optional
    copyRegion.dstOffset    = 0;  // Optional
    copyRegion.size         = size;

    vkCmdCopyBuffer       (commandBuffer, 
                           srcBuffer, 
                           dstBuffer, 
                           1, 
                           &copyRegion);
    endSingleTimeCommands (&commandBuffer, 
                           device);
}

void copyBufferToImage(Device device,
                       VkBuffer buffer, 
                       VkImage image, 
                       uint32_t width, 
                       uint32_t height, 
                       uint32_t layerCount) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device);
  
    VkBufferImageCopy region = {0};
    region.bufferOffset                    = 0;
    region.bufferRowLength                 = 0;
    region.bufferImageHeight               = 0;
  
    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = layerCount;
  
    region.imageOffset                     = {0, 0, 0};
    region.imageExtent                     = {width, height, 1};
  
    vkCmdCopyBufferToImage (commandBuffer,
                            buffer,
                            image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            1,
                            &region);
    endSingleTimeCommands  (&commandBuffer, 
                            device);
}

void copyIntoMappedMem(VulkanBuffer_T *buffer, void* srcData, size_t dataSize)
{
    memcpy(buffer->mapped, srcData, dataSize);
}

void createImageWithInfo(const VkImageCreateInfo *imageInfo,
                         const VkMemoryPropertyFlags properties,
                         VkImage *image,
                         VkDeviceMemory *imageMemory,
                         Device device) 
{
    if (vkCreateImage(getLogicalDevice(device), imageInfo, nullptr, image) != VK_SUCCESS) 
    {
        exit(1);
    }

    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo allocInfo{};

    vkGetImageMemoryRequirements(getLogicalDevice(device), *image, &memRequirements);
  
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, device);

    // TODO: stdlib shit
    if (vkAllocateMemory(getLogicalDevice(device), &allocInfo, nullptr, imageMemory) 
            != VK_SUCCESS){
        exit(1);
    }
  
    if (vkBindImageMemory(getLogicalDevice(device), *image, *imageMemory, 0) 
            != VK_SUCCESS){
        exit(1);
    }
}

static VkMemoryAllocateInfo memoryAllocateInfo(const VkMemoryRequirements memRequirements,
                                        const VkMemoryPropertyFlags properties, 
                                        const Device theGPU)
{
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, theGPU);
    return allocInfo;
}

static VkBufferCreateInfo bufferCreateInfo(const VkDeviceSize size, 
                                    const VkBufferUsageFlags usage)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    return bufferInfo;
}
