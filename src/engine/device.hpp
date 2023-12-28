#ifndef DEVICE
#define DEVICE

#include <vector>
#include <vulkan/vulkan_core.h>

#include "error_handling.h"
#include "defines.hpp"
#include "window.hpp"

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
    uint32_t queueCount;
}VkDeviceQueueCreateInfoArray;

typedef struct VulkanExtensions {
    const char **extensions;
    uint32_t count;
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

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
// Interface:
BBError deviceInit    (Device *device, BBWindow deviceWindow);
void    destroyDevice (Device device); 

// Sub-routines:
static void                    createInstance                    (Device theGPU);
static void                    setupDebugMessenger               (Device theGPU);
static void                    createSurface                     (Device theGPU);
static void                    pickPhysicalDevice                (Device theGPU);
static void                    createLogicalDevice               (Device theGPU);
static void                    createCommandPool                 (Device theGPU);

static SwapChainSupportDetails querySwapChainSupport             (const VkPhysicalDevice physicalDevice, 
                                                                  const Device theGPU);
static QueueFamilyIndices      findQueueFamilies                 (const VkPhysicalDevice physicalDevice, 
                                                                  const Device theGPU);
static bool                    isDeviceSuitable                  (const VkPhysicalDevice device, 
                                                                  const Device theGPU);
static bool                    checkDeviceExtensionSupport       (VkPhysicalDevice device);
static bool                    checkValidationLayerSupport       ();
static uint32_t                findMemoryType                    (uint32_t typeFilter, 
                                                                  VkMemoryPropertyFlags properties, 
                                                                  Device theGPU);
static void                    populateDebugMessengerCreateInfo  (VkDebugUtilsMessengerCreateInfoEXT &createInfo);
static void                    hasGflwRequiredInstanceExtensions ();
static BBError                 getRequiredInstanceExtensions     (VulkanExtensions *extensions);
static QueueFamilyIndices      findQueueFamilies                 (const VkPhysicalDevice device, 
                                                                  const Device theGPU);
static VkFormat                findSupportedFormat               (const std::vector<VkFormat> &candidates, 
                                                                  const VkImageTiling tiling, 
                                                                  const VkFormatFeatureFlags features,
                                                                  const Device theGPU);
// TODO: some global variables??? std::vectors??? fix this maybe.
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
extern VulkanExtensions deviceExtensions;

#endif
