#ifndef BB_VULKAN_LIB
#define BB_VULKAN_LIB

#define GLFW_INCLUDE_VULKAN
#include <string>

typedef void *BBWindow;
typedef void *BBEntity;

typedef int BBError;

BBWindow  openWindow       (int w, 
                            int h, 
                            std::string name);
void      runAppWithWindow (BBWindow mainWindow);

BBError   createEntity     (char *model, 
                            char *texture, 
                            char *vertShader, 
                            char *fragShader);
void      spawnEntity      (BBEntity entity, 
                            double worldCoords[3], 
                            double rotation);
void      printBBError     (BBError error);

#endif
