#ifndef INTERFACE
#define INTERFACE

#include <vulkan/vulkan_core.h>

#include <vector>
#include <array>

#include "entity.h"
#include "window.hpp"
#include "defines.hpp"
#include "main_loader.h"
#include "GLFW/glfw3.h"
#include "buffers.hpp"
#include "command_buffers.hpp"
#include "device.hpp"
#include "draw_frame.hpp"
#include "model.hpp"
#include "descriptor_sets.hpp"
#include "pipeline.hpp" 
#include "swap_chain.hpp"
#include "cleanup.hpp"
#include "images.hpp"

enum BBAPI BBAxis
{
    BB_AXIS_X = 2,
    BB_AXIS_Y = 4,
    BB_AXIS_Z = 8
};

BBAPI BBError createEntity     (BBEntity entity, 
                                char *model, 
                                char *textureDir, 
                                char *vertShader, 
                                char *fragShader);
BBAPI void    spawnEntity      (BBEntity entity, 
                                double *worldCoords, 
                                int rotation);
BBAPI void    initializeGFX    (BBWindow *mainWindow);
BBAPI void    runAppWithWindow (BBWindow *mainWindow);
BBAPI void    rotateEntity     (BBEntity entity, 
                                BBAxis axis);

#endif
