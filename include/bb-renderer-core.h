#ifndef BB_VULKAN_LIB
#define BB_VULKAN_LIB

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define BB_DEFINE_HANDLE(object) typedef struct object##_T *object;

BB_DEFINE_HANDLE(BBEntity)
BB_DEFINE_HANDLE(BBWindow)

typedef enum
{
    BB_AXIS_X = 2,
    BB_AXIS_Y = 4,
    BB_AXIS_Z = 8
}BBAxis;

// return 0 on success, -1 on error
int initializeGFX    (const BBWindow mainWindow);
int openWindow       (BBWindow *window,
                      const int w, 
                      const int h, 
                      const char *windowName);
int initEntity     (BBEntity *entity,
                      const char *model, 
                      const char *texture, 
                      const char *vertShader, 
                      const char *fragShader);
// runs a test scene in a single call to see if everything is working
void runAppWithWindow (BBWindow mainWindow);
// not implemented
void spawnEntity      (const BBEntity entity, 
                       const double worldCoords[3], 
                       const double rotation);
void rotateEntity     (BBEntity entity, 
                       BBAxis axis);

#endif
