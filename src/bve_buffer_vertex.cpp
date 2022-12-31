#include "bve_buffer_vertex.hpp"
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
}
