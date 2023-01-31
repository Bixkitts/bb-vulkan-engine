#ifndef CONFIG_DEVICE
#define CONFIG_DEVICE
#include <vector>
#include <vulkan/vulkan_core.h>
#include "device.hpp"
namespace config
{
    VkApplicationInfo appInfo();
    VkInstanceCreateInfo instanceCreateInfo(VkApplicationInfo &appInfo, std::vector<const char*> &extensions);
    VkDeviceQueueCreateInfo queueCreateInfo(
            std::vector<VkDeviceQueueCreateInfo> &queueCreateInfos, uint32_t &queueFamily, float &queuePriority );
    VkCommandPoolCreateInfo poolInfo(bve::QueueFamilyIndices &queueFamilyIndices);
    VkCommandBufferAllocateInfo commandBufferAllocInfo(bve::Device *theGPU);
    VkDeviceCreateInfo logicalCreateInfo(std::vector<VkDeviceQueueCreateInfo> &queueCreateInfos, VkPhysicalDeviceFeatures &deviceFeatures, const std::vector<const char *> &deviceExtensions );
}
#endif
