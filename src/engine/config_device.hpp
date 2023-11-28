#ifndef CONFIG_DEVICE
#define CONFIG_DEVICE

#include <vector>

#include <vulkan/vulkan_core.h>
#include "device.hpp"

// TODO: return types
VkApplicationInfo           appInfo                ();
VkCommandPoolCreateInfo     poolCreateInfo         (QueueFamilyIndices queueFamilyIndices);
VkCommandBufferAllocateInfo commandBufferAllocInfo (Device theGPU);
VkDeviceQueueCreateInfo     queueCreateInfo        (std::vector<VkDeviceQueueCreateInfo> queueCreateInfos, 
                                                    uint32_t queueFamily, 
                                                    float queuePriority );
VkInstanceCreateInfo        instanceCreateInfo     (VkApplicationInfo appInfo, 
                                                    std::vector<const char*> extensions);
VkDeviceCreateInfo          logicalCreateInfo      (std::vector<VkDeviceQueueCreateInfo> queueCreateInfos, 
                                                    VkPhysicalDeviceFeatures deviceFeatures, 
                                                    const std::vector<const char *> deviceExtensions );
#endif
