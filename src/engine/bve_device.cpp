#include "bve_device.hpp"
#include "bve_buffers.hpp"
#include "config_device.hpp"
#include "bve_model.hpp"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace bve {

// local callback functions
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) 
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) 
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } 
    else 
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) 
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
// end of local callback functions

Device* deviceInit(BveWindow* deviceWindow) 
{
    Device* theGPU = new Device;
    theGPU->window = deviceWindow;
    createInstance(theGPU);
    setupDebugMessenger(theGPU); 
    bve::createWindowSurface(theGPU->window, theGPU->instance, &theGPU->surface_);
    pickPhysicalDevice(theGPU);

    createLogicalDevice(theGPU);
    createCommandPool(theGPU);
    return theGPU;
}

void destroyDevice(Device* device) 
{
    vkDestroyCommandPool(device->logical, device->commandPool, nullptr);
  
    if (enableValidationLayers) 
    {
        DestroyDebugUtilsMessengerEXT(device->instance, device->debugMessenger, nullptr);
    }
  
    vkDestroySurfaceKHR(device->instance, device->surface_, nullptr);
    vkDestroyInstance(device->instance, nullptr);
    vkDestroyDevice(device->logical, nullptr);
    delete device;
}

static void createInstance(Device* theGPU) 
{
    if (enableValidationLayers && !checkValidationLayerSupport()) 
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = config::appInfo();

    auto extensions = getRequiredExtensions();
    VkInstanceCreateInfo createInfo = config::instanceCreateInfo(appInfo, extensions);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) 
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT* )&debugCreateInfo;
    } 
    else 
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &theGPU->instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    hasGflwRequiredInstanceExtensions();
}

static void pickPhysicalDevice(Device* theGPU) 
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(theGPU->instance, &deviceCount, nullptr);
    if (deviceCount == 0) 
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::cout << "Device count: " << deviceCount << std::endl;
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(theGPU->instance, &deviceCount, devices.data());
  
    for (const auto &device : devices) 
    {
        if (isDeviceSuitable(device, theGPU)) 
        {
          theGPU->physical = device;
          break;
      }
    }
  
    if (theGPU->physical == VK_NULL_HANDLE) 
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
  
    vkGetPhysicalDeviceProperties(theGPU->physical, &theGPU->physicalProperties);
    std::cout << "physical device: " << theGPU->physicalProperties.deviceName << std::endl;
}

static void createLogicalDevice(Device* theGPU) 
{
    QueueFamilyIndices indices = findQueueFamilies(theGPU->physical, theGPU);
  
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
  
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) 
    {
        VkDeviceQueueCreateInfo queueCreateInfo = config::queueCreateInfo(queueCreateInfos, queueFamily, queuePriority);
    }
  
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
  
    VkDeviceCreateInfo createInfo = config::logicalCreateInfo(queueCreateInfos, deviceFeatures, deviceExtensions);
  
    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
    if (enableValidationLayers) 
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } 
    else 
    {
        createInfo.enabledLayerCount = 0;
    }
  
    if (vkCreateDevice(theGPU->physical, &createInfo, nullptr, &theGPU->logical) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
  
    vkGetDeviceQueue(theGPU->logical, indices.graphicsFamily, 0, &theGPU->graphicsQueue_);
    vkGetDeviceQueue(theGPU->logical, indices.presentFamily, 0, &theGPU->presentQueue_);
}

static void createCommandPool(Device* theGPU) 
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(theGPU->physical, theGPU);
  
    VkCommandPoolCreateInfo poolInfo = config::poolInfo(queueFamilyIndices);

    if (vkCreateCommandPool(theGPU->logical, &poolInfo, nullptr, &theGPU->commandPool) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create command pool!");
    }
}


bool isDeviceSuitable(VkPhysicalDevice device, Device* theGPU) 
{
    QueueFamilyIndices indices = findQueueFamilies(device, theGPU);
  
    bool extensionsSupported = checkDeviceExtensionSupport(device);
  
    bool swapChainAdequate = false;
    if (extensionsSupported) 
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, theGPU);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
  
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
  
    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
        supportedFeatures.samplerAnisotropy;
}

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) 
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;  // Optional
}

static void setupDebugMessenger(Device* theGPU)
{
    if (!enableValidationLayers) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(theGPU->instance, &createInfo, nullptr, &theGPU->debugMessenger) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

bool checkValidationLayerSupport() 
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) 
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) 
            {
                layerFound = true;
                break;
            }
    }

    if (!layerFound) 
    {
        return false;
    }
  }

  return true;
}

std::vector<const char* > getRequiredExtensions()  
{
    uint32_t glfwExtensionCount = 0;
    const char* *glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  
    std::vector<const char* > extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
  
    return extensions;
}

void hasGflwRequiredInstanceExtensions() 
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
  
    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> available;
    for (const auto &extension : extensions) 
    {
        std::cout << "\t" << extension.extensionName << std::endl;
        available.insert(extension.extensionName);
    }
  
    std::cout << "required extensions:" << std::endl;
    auto requiredExtensions = getRequiredExtensions();
    for (const auto &required : requiredExtensions)  
    {
        std::cout << "\t" << required << std::endl;
        if (available.find(required) == available.end()) {
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) 
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        availableExtensions.data());
  
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
  
    for (const auto &extension : availableExtensions) 
    {
        requiredExtensions.erase(extension.extensionName);
    }
  
    return requiredExtensions.empty();
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, Device* theGPU) 
{
    QueueFamilyIndices indices;
  
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
  
    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.graphicsFamilyHasValue = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, theGPU->surface_, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
            indices.presentFamilyHasValue = true;
        }
        if (indices.isComplete()) {
            break;
        }
    
        i++;
    }
  
    return indices;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, Device* theGPU) 
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, theGPU->surface_, &details.capabilities);
  
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, theGPU->surface_, &formatCount, nullptr);
  
    if (formatCount != 0) 
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, theGPU->surface_, &formatCount, details.formats.data());
    }
  
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, theGPU->surface_, &presentModeCount, nullptr);
  
    if (presentModeCount != 0) 
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            theGPU->surface_,
            &presentModeCount,
            details.presentModes.data());
    }
    return details;
}

VkFormat findSupportedFormat(
    const std::vector<VkFormat> &candidates, 
    VkImageTiling tiling, 
    VkFormatFeatureFlags features,
    Device* theGPU) 
{
    for (VkFormat format : candidates) 
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(theGPU->physical, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
        {
            return format;
        } 
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
        {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Device* theGPU) 
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(theGPU->physical, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
        {
            return i;
        }
    }
  
    throw std::runtime_error("failed to find suitable memory type!");
}


VkCommandBuffer beginSingleTimeCommands(Device* theGPU) 
{
    VkCommandBufferAllocateInfo allocInfo = config::commandBufferAllocInfo(theGPU);

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(theGPU->logical, &allocInfo, &commandBuffer);
  
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer, Device* theGPU) 
{
    vkEndCommandBuffer(commandBuffer);
  
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
  
    vkQueueSubmit(theGPU->graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(theGPU->graphicsQueue_);
  
    vkFreeCommandBuffers(theGPU->logical, theGPU->commandPool, 1, &commandBuffer);
}



} 


