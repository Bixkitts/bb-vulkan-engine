#include "bve_model.hpp"

#include <cassert>

namespace bve
{

    BveModel *createBveModel(BveDevice *device, std::vector<Vertex> &vertices)
    {
        BveModel *model = new BveModel{device};
        createVertexBuffers(vertices);

        return model;
    } 

    void destroyBveModel(BveModel *model)
    {
        vkDestroyBuffer(model->device->device_, model->vertexBuffer, nullptr);
        vkFreeMemory(model->device->device_, model->vertexMemory, nullptr);

    }

    void createVertexBuffers(BveModel *model, const std::vector<Vertex> &vertices)
    {
       model->vertexCount = static_cast<uint32_t>(vertices.size());
       assert(model->vertexCount >= 3 && "Vertex count must be at least 3");
       VkDeviceSize bufferSize = sizeof(vertices[0]) * model->vertexCount;

    }

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);


}
