#ifndef INTERFACE
#define INTERFACE

#include <vulkan/vulkan_core.h>

#include "entity.h"
#include "window.hpp"
#include "defines.hpp"

BBAPI BBError createEntity     (BBEntity entity, 
                                const char *model, 
                                const char *textureDir, 
                                const char *vertShader, 
                                const char *fragShader);
BBAPI void    spawnEntity      (BBEntity entity, 
                                double worldCoords[3], 
                                double rotation);
BBAPI void    initializeGFX    (BBWindow mainWindow);
BBAPI void    runAppWithWindow (BBWindow mainWindow);
BBAPI void    rotateEntity     (BBEntity entity, 
                                BBAxis axis);

#endif
