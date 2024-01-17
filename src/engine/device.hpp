#ifndef DEVICE
#define DEVICE

#include <vulkan/vulkan_core.h>

#include "error_handling.h"
#include "defines.hpp"
#include "window.hpp"
#include <vector>

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
}SwapChainSupportDetails;

// TODO: CPP shit
typedef struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool     graphicsFamilyHasValue = false;
    bool     presentFamilyHasValue = false;
    bool     isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
}QueueFamilyIndices;

typedef struct VkDeviceQueueCreateInfoArray{
    VkDeviceQueueCreateInfo createInfos[QUEUES_MAX];
    uint32_t                queueCount;
}VkDeviceQueueCreateInfoArray;

typedef struct VulkanExtensions {
    char     **extensions;
    uint32_t   count;
} VulkanExtensions;

typedef struct Device_T {
    VkInstance                  instance;
    VkDebugUtilsMessengerEXT    debugMessenger;
    VkPhysicalDevice            physical = VK_NULL_HANDLE;
    BBWindow                    window;
    VkCommandPool               commandPool;
    VkPhysicalDeviceProperties  physicalProperties;

    VkDevice                    logical;
    VkSurfaceKHR                surface_;
    VkQueue                     graphicsQueue_;
    VkQueue                     presentQueue_;
}Device_T, *Device;

BBError                 deviceInit            (Device *device, 
                                               const BBWindow deviceWindow);
void                    destroyDevice         (Device *device); 
uint32_t                findMemoryType        (const uint32_t typeFilter, 
                                               const VkMemoryPropertyFlags properties, 
                                               const Device device);
SwapChainSupportDetails querySwapChainSupport (const Device device);
QueueFamilyIndices      findQueueFamilies     (const Device device);
VkFormat                findSupportedFormat   (const std::vector<VkFormat> &candidates, 
                                               const VkImageTiling tiling, 
                                               const VkFormatFeatureFlags features,
                                               const Device device);

#endif
