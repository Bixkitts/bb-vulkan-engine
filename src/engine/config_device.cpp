#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "config_device.hpp"

VkApplicationInfo createAppInfo()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "VulkanEngine App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "No Engine";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_0;
    return appInfo;
}
VkInstanceCreateInfo instanceCreateInfo(VkApplicationInfo appInfo, 
                                        std::vector<const char*> extensions)
{
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    return createInfo;
}
VkDeviceQueueCreateInfo createQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo> queueCreateInfos, 
                                              uint32_t queueFamily, 
                                              float queuePriority)
{
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
    return queueCreateInfo;
}
VkCommandPoolCreateInfo poolCreateInfo(QueueFamilyIndices queueFamilyIndices)
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags            =
    VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 
    | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    return poolInfo;
}
VkDeviceCreateInfo logicalCreateInfo(std::vector<VkDeviceQueueCreateInfo> queueCreateInfos, 
                                     VkPhysicalDeviceFeatures deviceFeatures, 
                                     const std::vector<const char *> deviceExtensions)
{
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos       = queueCreateInfos.data();
    createInfo.pEnabledFeatures        = &deviceFeatures;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    return createInfo;
}
