#include "config_buffers.hpp"
VkMemoryAllocateInfo memoryAllocateInfo(VkMemoryRequirements &memRequirements,VkMemoryPropertyFlags &properties, Device *theGPU)
{
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, theGPU);
    return allocInfo;
}
VkBufferCreateInfo bufferCreateInfo(VkDeviceSize &size, VkBufferUsageFlags &usage)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    return bufferInfo;
}
