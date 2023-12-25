#ifndef DEVICE
#define DEVICE

#include <vector>

#include "error_handling.h"
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
typedef struct GLExtensions {
    const char **extensions;
    uint32_t count;
} GLExtensions;
typedef struct Device_T
{
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
static void createInstance      (Device theGPU);
static void setupDebugMessenger (Device theGPU);
static void createSurface       (Device theGPU);
static void pickPhysicalDevice  (Device theGPU);
static void createLogicalDevice (Device theGPU);
static void createCommandPool   (Device theGPU);


// helper functions
bool                      isDeviceSuitable                  (VkPhysicalDevice device, 
                                                             Device theGPU);
bool                      checkDeviceExtensionSupport       (VkPhysicalDevice device);
bool                      checkValidationLayerSupport       ();
uint32_t                  findMemoryType                    (uint32_t typeFilter, 
                                                             VkMemoryPropertyFlags properties, 
                                                             Device theGPU);
void                      populateDebugMessengerCreateInfo  (VkDebugUtilsMessengerCreateInfoEXT &createInfo);
void                      hasGflwRequiredInstanceExtensions ();
BBError                   getRequiredExtensions             (GLExtensions *extensions);
QueueFamilyIndices        findQueueFamilies                 (VkPhysicalDevice device, 
                                                             Device theGPU);
SwapChainSupportDetails   querySwapChainSupport             (VkPhysicalDevice physicalDevice, 
                                                             Device theGPU);
QueueFamilyIndices        findQueueFamilies                 (VkPhysicalDevice physicalDevice, 
                                                             Device theGPU);
VkFormat                  findSupportedFormat               (const std::vector<VkFormat> &candidates, 
                                                             VkImageTiling tiling, 
                                                             VkFormatFeatureFlags features,
                                                             Device theGPU);
// TODO: some global variables??? std::vectors??? fix this maybe.
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#endif
