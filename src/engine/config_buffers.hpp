#ifndef CONFIG_BUFFERS
#define CONFIG_BUFFERS
#include <vulkan/vulkan.h>
#include "device.hpp"

VkMemoryAllocateInfo memoryAllocateInfo(VkMemoryRequirements &memRequirements,VkMemoryPropertyFlags &properties, Device *theGPU);
VkBufferCreateInfo bufferCreateInfo(VkDeviceSize &size, VkBufferUsageFlags &usage);

#endif
