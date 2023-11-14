#ifndef BVE_MODEL
#define BVE_MODEL

#include <cassert>
#include <vulkan/vulkan_core.h>
#include <cstring>
#include <iostream>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#include "vertex.hpp"
#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

struct Model
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indeces;
};

std::vector<Model*> loadModels(char *dir);
Model *loadModel(char *dir);


Model *createModel(Device *device, std::vector<Vertex> &vertices); 
void bindModel(Model *model, VkCommandBuffer commandBuffer);
void drawModel(Model *model, VkCommandBuffer commandBuffer);

#endif
