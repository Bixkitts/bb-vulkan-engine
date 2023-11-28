#include "config_buffers.hpp"
// TODO: in parameters
VkMemoryAllocateInfo memoryAllocateInfo(const VkMemoryRequirements memRequirements,
                                        const VkMemoryPropertyFlags properties, 
                                        const Device theGPU)
{
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, theGPU);
    return allocInfo;
}

VkBufferCreateInfo bufferCreateInfo(const VkDeviceSize size, 
                                    const VkBufferUsageFlags usage)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    return bufferInfo;
}
