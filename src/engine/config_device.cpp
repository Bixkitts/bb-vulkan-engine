#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "config_device.hpp"

void createAppInfo(VkApplicationInfo *appInfo)
{
    *appInfo                    = {};
    appInfo->sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo->pApplicationName   = "VulkanEngine App";
    appInfo->applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo->pEngineName        = "No Engine";
    appInfo->engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo->apiVersion         = VK_API_VERSION_1_0;
}
void createInstanceCreateInfo(VkInstanceCreateInfo *createInfo,
                              const VkApplicationInfo *appInfo, 
                              const VulkanExtensions *extensions)
{
    *createInfo                         = {};
    createInfo->sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo->pApplicationInfo        = appInfo;
    createInfo->enabledExtensionCount   = extensions->count;
    createInfo->ppEnabledExtensionNames = extensions->extensions;
}
void createQueueCreateInfo(VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                           const uint32_t queueFamily, 
                           const float *queuePriority)
{
    queueCreateInfos->createInfos[0].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos->createInfos[0].queueFamilyIndex = queueFamily;
    queueCreateInfos->createInfos[0].queueCount       = 1;
    queueCreateInfos->createInfos[0].pQueuePriorities = queuePriority;

    queueCreateInfos->queueCount                      = 1;
}
VkCommandPoolCreateInfo createCommandPoolCreateInfo(const QueueFamilyIndices queueFamilyIndices)
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags            =
    VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 
    | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    return poolInfo;
}
void createDeviceCreateInfo(VkDeviceCreateInfo *createInfo,
                            const VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                            const VkPhysicalDeviceFeatures *deviceFeatures, 
                            const VulkanExtensions *deviceExtensions)
{
    *createInfo = {};
    createInfo->sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo->queueCreateInfoCount    = queueCreateInfos->queueCount;
    createInfo->pQueueCreateInfos       = queueCreateInfos->createInfos;
    createInfo->pEnabledFeatures        = deviceFeatures;
    createInfo->enabledExtensionCount   = deviceExtensions->count;
    createInfo->ppEnabledExtensionNames = deviceExtensions->extensions;
}
