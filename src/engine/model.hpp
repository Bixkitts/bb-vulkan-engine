#ifndef BVE_MODEL
#define BVE_MODEL

#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>
#include <stdio.h>

#include "defines.hpp"
#include "error_handling.h"
#include "vertex.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

typedef struct Model_T Model_T;
BB_OPAQUE_HANDLE(Model);

BBError    loadModel           (Model *model, const char *dir);

Vertex    *getModelVerts       (Model model);
VertIndex *getModelIndeces     (Model model);
uint32_t   getModelIndexCount  (Model model);
uint32_t   getModelVertexCount (Model model);

void       bindModel   (Model model, VkCommandBuffer commandBuffer);
void       drawModel   (Model model, VkCommandBuffer commandBuffer);

#endif
