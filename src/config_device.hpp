#ifndef CONFIG_DEVICE
#define CONFIG_DEVICE
#include <vector>
#include <vulkan/vulkan_core.h>
#include "bve_device.hpp"
namespace config
{
    VkApplicationInfo appInfo();
    VkInstanceCreateInfo instanceCreateInfo(VkApplicationInfo &appInfo, std::vector<const char*> &extensions);
    VkDeviceQueueCreateInfo queueCreateInfo(
            std::vector<VkDeviceQueueCreateInfo> &queueCreateInfos, uint32_t &queueFamily, float &queuePriority );
    VkMemoryAllocateInfo memoryAllocateInfo(VkMemoryRequirements &memRequirements,VkMemoryPropertyFlags &properties, bve::Device *theGPU);
    VkCommandPoolCreateInfo poolInfo(bve::QueueFamilyIndices &queueFamilyIndices);
    VkCommandBufferAllocateInfo commandBufferAllocInfo(bve::Device *theGPU);
    VkBufferCreateInfo bufferCreateInfo(VkDeviceSize &size, VkBufferUsageFlags &usage);
}
#endif
