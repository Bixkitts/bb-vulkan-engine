#pragma once
#include <vulkan/vulkan.h>
#include "device.hpp"

namespace config
{

    VkMemoryAllocateInfo memoryAllocateInfo(VkMemoryRequirements &memRequirements,VkMemoryPropertyFlags &properties, bve::Device *theGPU);
    VkBufferCreateInfo bufferCreateInfo(VkDeviceSize &size, VkBufferUsageFlags &usage);

}
