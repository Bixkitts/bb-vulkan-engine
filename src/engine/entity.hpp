#ifndef ENTITY
#define ENTITY

#include "model.hpp"
#include "buffers.hpp"
namespace bve
{
typedef struct BBEntity
{
   Model *model; 
   PerObjectMatrices *modelMatrix;
   // next I need to associate pipeline
   // and resources for the entity
} BBEntity;


}



#endif
