#include "bve_model.hpp"

#include <cassert>
#include <vulkan/vulkan_core.h>
#include <cstring>
#include <iostream>

namespace bve
{

    Model* createBveModel(Device* device, std::vector<Vertex> &vertices)
    {
        Model* model = new Model{device};
        createVertexBuffers(model, vertices);

        return model;
    } 

    void destroyBveModel(Model* model)
    {
        vkDestroyBuffer(model->device->device_, model->vertexBuffer, nullptr);
        vkFreeMemory(model->device->device_, model->vertexMemory, nullptr);

    }

    void createVertexBuffers(Model* model, const std::vector<Vertex> &vertices)
    {
        model->vertexCount = static_cast<uint32_t>(vertices.size());
        assert(model->vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0])*  model->vertexCount;
        createBuffer(bufferSize,
               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               model->vertexBuffer,
               model->vertexMemory,
               model->device);

        void* data;
        vkMapMemory(model->device->device_, model->vertexMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(model->device->device_, model->vertexMemory);
    }

    void drawModel(Model* model, VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, model->vertexCount, 1, 0, 0);
    }

    void bindModel(Model* model, VkCommandBuffer commandBuffer)
    {
        std::cout<<"Bind Model called";
        VkBuffer buffers[] = {model->vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0 ,1, buffers, offsets); 
    }

    std::vector<VkVertexInputBindingDescription> getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
            bindingDescriptions[0].binding = 0;
            bindingDescriptions[0].stride = sizeof(Vertex);
            bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0;
        return attributeDescriptions;
    }

    std::vector<Model*> loadModels(Device* device)
    {
        std::cout<<"Loading models....\n";

        //placeholder model loader
        std::vector<Vertex> vertices 
        {
            {{0.0f, -0.5f}},
            {{0.5f, 0.5f}},
            {{-0.5f, 0.5f}}
        };
        Model* model = createBveModel(device, vertices);
        //placeholder model loader
        std::vector<Vertex> vertices2 
        {
            {{-0.9f, -0.9f}},
            {{-0.9f, -0.8f}},
            {{-0.8f, -0.85f}}
        };
        Model* model2 = createBveModel(device, vertices2);

        
        std::vector<Model*> models;
        models.push_back(model2); 
        models.push_back(model); 
        
        return models;
    }


}
