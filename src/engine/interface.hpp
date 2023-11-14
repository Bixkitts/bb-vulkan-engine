#ifndef INTERFACE
#define INTERFACE

#include "entity.hpp"
#include "window.hpp"
#include "defines.hpp"
enum BBAPI BBAxis
{
    BB_AXIS_X = 2,
    BB_AXIS_Y = 4,
    BB_AXIS_Z = 8
};

BBAPI void runAppWithWindow(BBWindow* mainWindow);
BBAPI void rotateEntity(P_BBEntity entity, BBAxis axis);

#endif
