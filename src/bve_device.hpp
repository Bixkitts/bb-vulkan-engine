#pragma once

#include "bve_window.hpp"

// std lib headers
#include <string>
#include <vector>

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
struct BveDevice
{
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  BveWindow *window;
  VkCommandPool commandPool;
  VkPhysicalDeviceProperties deviceproperties;

  VkDevice device_;
  VkSurfaceKHR surface_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
};
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif
//-----Constructor-------------
  BveDevice *bveDeviceInit(BveWindow *deviceWindow);
//-----------------------------
//
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, BveDevice *theGPU);
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, BveDevice *theGPU);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevicei, BveDevice *theGPU);

  VkFormat findSupportedFormat(
      const std::vector<VkFormat> &candidates, 
      VkImageTiling tiling, 
      VkFormatFeatureFlags features,
      BveDevice *theGPU);

  // Buffer Helper Functions
  void createBuffer(
      VkDeviceSize size,
      VkBufferUsageFlags usage,
      VkMemoryPropertyFlags properties,
      VkBuffer &buffer,
      VkDeviceMemory &bufferMemory,
      BveDevice *theGPU);

  VkCommandBuffer beginSingleTimeCommands(BveDevice *theGPU);
  void endSingleTimeCommands(VkCommandBuffer commandBuffer, BveDevice *theGPU);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void copyBufferToImage(
      VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

  void createImageWithInfo(
      const VkImageCreateInfo &imageInfo,
      VkMemoryPropertyFlags properties,
      VkImage &image,
      VkDeviceMemory &imageMemory);
// private:
  static void createInstance(BveDevice *theGPU);
  static void setupDebugMessenger(BveDevice *theGPU);
  static void createSurface(BveDevice *theGPU);
  static void pickPhysicalDevice(BveDevice *theGPU);
  static void createLogicalDevice(BveDevice *theGPU);
  static void createCommandPool(BveDevice *theGPU);

  // helper functions
  bool isDeviceSuitable(VkPhysicalDevice device, BveDevice *theGPU);
  std::vector<const char *> getRequiredExtensions();
  bool checkValidationLayerSupport();
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, BveDevice *theGPU);
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  void hasGflwRequiredInstanceExtensions();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

}


