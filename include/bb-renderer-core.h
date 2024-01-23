#ifndef BB_VULKAN_LIB
#define BB_VULKAN_LIB

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define BB_DEFINE_HANDLE(object) typedef struct object##_T *object;

BB_DEFINE_HANDLE(BBRenderObject)
BB_DEFINE_HANDLE(BBWindow)

// Bitwise OR these to use multiple at once
typedef enum
{
    BB_AXIS_X = 1,
    BB_AXIS_Y = 2,
    BB_AXIS_Z = 4
}BBAxis;

// All return 0 on success, -1 on error
int initializeGFX      (const BBWindow mainWindow);
int openWindow         (BBWindow *window,
                        const int w, 
                        const int h, 
                        const char *windowName);
int createRenderObject (BBRenderObject *object,
                        const char *model, 
                        const char *texture, 
                        const char *vertShader, 
                        const char *fragShader);
// runs a test scene in a single call to see if everything is working
void runAppWithWindow  (BBWindow mainWindow);

/* ---   Not implemented  ---  */
void rotateObject      (BBRenderObject object, 
                        size_t magnitude,
                        BBAxis axis);
void transformObject   (BBRenderObject object,
                        size_t magnitude,
                        BBAxis axis);
void scaleObject       (BBRenderObject object,
                        size_t magnitude,
                        BBAxis axis);
void rotateCamera      (size_t magnitude,
                        BBAxis axis);
void transformCamera   (size_t magnitude,
                        BBAxis axis);

// Primitives...
int createQuad         (BBRenderObject *object,
                        size_t width,
                        size_t height,
                        const char *texture,
                        const char *vertShader, 
                        const char *fragShader);

// Print last error to stdout (or any file handle??)
void printLastError    (void);
/* ----------------------------- */

#endif
