#ifndef ENTITY
#define ENTITY

#include "descriptor_sets.hpp"
#include "model.hpp"
#include "buffers.hpp"
#include "images.hpp"
#include "pipeline.hpp"
#include <vulkan/vulkan_core.h>

typedef struct BBEntity
{
    Device *device;       
    Model *model; 
    // Access the uniform buffers as an array
    UniformBuffer *transformBuffer;     // 0
    UniformBuffer *cameraBuffer;        // 1
    // -------------------------------------
    VertexBuffer *vBuffer;
    IndexBuffer *iBuffer;
    VulkanImage *texture;
    BBDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet *descriptorSet;
    GraphicsPipeline *pipeline;
} BBEntity;

#endif
