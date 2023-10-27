#ifndef ENTITY
#define ENTITY

#include "model.hpp"
#include "buffers.hpp"
#include "images.hpp"

typedef struct BBEntity
{
   Model *model; 
   PerObjectMatrices *modelMatrix;
   // next I need to associate pipeline
   // and resources for the entity
   VulkanImage *texture;
} BBEntity;

#endif
