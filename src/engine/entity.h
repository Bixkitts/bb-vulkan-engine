#ifndef BB_ENTITY
#define BB_ENTITY

#include "descriptor_sets.hpp"
#include "model.hpp"
#include "buffers.hpp"
#include "images.hpp"
#include "pipeline.hpp"
#include <vulkan/vulkan_core.h>

typedef struct BBEntity
{
    Device                *device;       
    Model                 *model;

    UniformBuffer         *uBuffer;

    VertexBuffer          *vBuffer;
    IndexBuffer           *iBuffer;
    VulkanImage           *texture;
    BBDescriptorSetLayout  descriptorSetLayout;
    VkDescriptorSet       *descriptorSet;
    GraphicsPipeline      *pipeline;
} BBEntity, *P_BBEntity;

#endif
