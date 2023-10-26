#ifndef BB_VULKAN_LIB
#define BB_VULKAN_LIB

#define GLFW_INCLUDE_VULKAN
#include <string>

typedef struct BBWindow BBWindow;
typedef struct BBEntity BBEntity;

BBWindow* openWindow(int w, int h, std::string name);
void runAppWithWindow(BBWindow* mainWindow);

BBEntity createEntity(char *model, char *texture, char *vertShader, char *fragShader);
void *spawnEntity(BBEntity entity, double worldCoords[3], double rotation);




#endif
