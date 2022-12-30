#pragma once


#include <string>
#include <vector>
#include "bve_window.hpp"


namespace bve {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};
struct Device
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physical = VK_NULL_HANDLE;
    BveWindow *window;
    VkCommandPool commandPool;
    VkPhysicalDeviceProperties physicalProperties;

    VkDevice logical;
    VkSurfaceKHR surface_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;
};
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
// Interface:
    Device *deviceInit(BveWindow *deviceWindow);

// Sub-routines:
    static void createInstance(Device *theGPU);
    static void setupDebugMessenger(Device *theGPU);
    static void createSurface(Device *theGPU);
    static void pickPhysicalDevice(Device *theGPU);
    static void createLogicalDevice(Device *theGPU);
    static void createCommandPool(Device *theGPU);

// Buffer Helper Functions
    void createDeviceBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer &buffer,
        VkDeviceMemory &bufferMemory,
        Device *theGPU);

    VkCommandBuffer beginSingleTimeCommands(Device *theGPU);
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, Device *theGPU);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

    void createImageWithInfo(
        const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory,
        Device *theGPU);

// helper functions
    bool isDeviceSuitable(VkPhysicalDevice device, Device *theGPU);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool checkValidationLayerSupport();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Device *theGPU);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void hasGflwRequiredInstanceExtensions();
    std::vector<const char *> getRequiredExtensions();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, Device *theGPU);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, Device *theGPU);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, Device *theGPU);
    VkFormat findSupportedFormat(
        const std::vector<VkFormat> &candidates, 
        VkImageTiling tiling, 
        VkFormatFeatureFlags features,
        Device *theGPU);

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
//Memory mapping functions

}


