#ifndef CONFIG_BUFFERS
#define CONFIG_BUFFERS
#include <vulkan/vulkan.h>
#include "device.hpp"

VkMemoryAllocateInfo memoryAllocateInfo (const VkMemoryRequirements memRequirements,
                                         const VkMemoryPropertyFlags properties, 
                                         const Device theGPU);
VkBufferCreateInfo   bufferCreateInfo   (const VkDeviceSize size, 
                                         const VkBufferUsageFlags usage);

#endif
