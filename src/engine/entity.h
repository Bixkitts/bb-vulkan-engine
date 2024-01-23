#ifndef BB_ENTITY
#define BB_ENTITY

#include "defines.hpp"
#include "descriptor_sets.hpp"
#include "model.hpp"
#include "buffers.hpp"
#include "images.hpp"
#include "pipeline.hpp"
#include <vulkan/vulkan_core.h>

typedef struct RenderObject_T
{
    Device                 device;       
    Model                  model;
    // The amount of uniform buffers in the array is
    // currently always MAX_FRAMES_IN_FLIGHT
    UniformBuffer          uBuffers[MAX_FRAMES_IN_FLIGHT];
    VertexBuffer           vBuffer;
    IndexBuffer            iBuffer;
    VulkanImage            texture;
    BBDescriptorSetLayout  descriptorSetLayout;
    VkDescriptorSet        descriptorSet;
    GraphicsPipeline       pipeline;
} RenderObject_T;

#endif
