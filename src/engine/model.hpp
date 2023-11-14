#ifndef BVE_MODEL
#define BVE_MODEL

#include <cassert>
#include <vulkan/vulkan_core.h>
#include <cstring>
#include <iostream>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#include "error_handling.h"
#include "vertex.hpp"
#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

struct Model
{
    // TODO:
    // If I'm going to store vertex attribute stuff, it ought to go here.
    Vertex *vertices;
    uint32_t vertexCount;
    uint32_t *indeces;
    uint32_t indexCount;
};

BBError loadModel(Model *model, char *dir);

Model *createModel(Device *device, std::vector<Vertex> &vertices); 
void bindModel(Model *model, VkCommandBuffer commandBuffer);
void drawModel(Model *model, VkCommandBuffer commandBuffer);

#endif
