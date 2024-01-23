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

typedef struct {
    VkDeviceQueueCreateInfo createInfos[QUEUES_MAX];
    uint32_t                queueCount;
}VkDeviceQueueCreateInfoArray;

typedef struct {
    char     **extensions;
    uint32_t   count;
} VulkanExtensions;

typedef struct Device_T Device_T;

BB_OPAQUE_HANDLE(Device);

BBError                    createDevice             (Device *device, 
                                                     const BBWindow deviceWindow);
void                       destroyDevice            (Device *device); 
uint32_t                   findMemoryType           (const uint32_t typeFilter, 
                                                     const VkMemoryPropertyFlags properties, 
                                                     Device device);
VkDevice                   getLogicalDevice         (Device device);
VkCommandPool              getDevCommandPool        (Device device);  
VkQueue                    getDevGraphicsQueue      (Device device);
VkQueue                    getDevPresentQueue       (Device device);
VkSurfaceKHR               getDevVkSurface          (Device device);
VkPhysicalDeviceProperties getDevPhysicalProperties (Device device);
SwapChainSupportDetails    querySwapChainSupport    (Device device);
QueueFamilyIndices         findQueueFamilies        (Device device);
VkFormat                   findSupportedFormat      (const std::vector<VkFormat> &candidates, 
                                                     const VkImageTiling tiling, 
                                                     const VkFormatFeatureFlags features,
                                                     Device device);

#endif
