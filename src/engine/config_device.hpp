#ifndef CONFIG_DEVICE
#define CONFIG_DEVICE

#include <vulkan/vulkan_core.h>
#include "device.hpp"

// TODO: return types
void                        createAppInfo               (VkApplicationInfo *appInfo);
VkCommandPoolCreateInfo     createCommandPoolCreateInfo (const QueueFamilyIndices queueFamilyIndices);
VkCommandBufferAllocateInfo commandBufferAllocInfo      (Device theGPU);
void                        createQueueCreateInfo       (VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                                                         uint32_t queueFamily, 
                                                         const float *queuePriority );
void                        createInstanceCreateInfo    (VkInstanceCreateInfo *createInfo,
                                                         const VkApplicationInfo *appInfo, 
                                                         const VulkanExtensions *extensions);
void                        createDeviceCreateInfo      (VkDeviceCreateInfo *createInfo,
                                                         const VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                                                         const VkPhysicalDeviceFeatures *deviceFeatures, 
                                                         const VulkanExtensions *deviceExtensions);
#endif
