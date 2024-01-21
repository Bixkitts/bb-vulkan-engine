#ifndef BVE_MODEL
#define BVE_MODEL

#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>
#include <stdio.h>

#include "error_handling.h"
#include "vertex.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

typedef struct
{
    // TODO:
    // If I'm going to store vertex attribute stuff, it ought to go here.
    Vertex    *vertices;
    VertIndex *indeces;
    uint32_t   vertexCount;
    uint32_t   indexCount;
}Model_T, *Model;

BBError loadModel   (Model model, const char *dir);

void    bindModel   (Model model, VkCommandBuffer commandBuffer);
void    drawModel   (Model model, VkCommandBuffer commandBuffer);

#endif
