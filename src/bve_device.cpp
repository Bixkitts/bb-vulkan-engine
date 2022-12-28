#include "bve_device.hpp"

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

void BveDeviceDestroy(Device* theGPU) 
{
    vkDestroyCommandPool(theGPU->device_, theGPU->commandPool, nullptr);
    vkDestroyDevice(theGPU->device_, nullptr);
  
    if (enableValidationLayers) 
    {
        DestroyDebugUtilsMessengerEXT(theGPU->instance, theGPU->debugMessenger, nullptr);
    }
  
    vkDestroySurfaceKHR(theGPU->instance, theGPU->surface_, nullptr);
    vkDestroyInstance(theGPU->instance, nullptr);
}

static void createInstance(Device* theGPU) 
{
    if (enableValidationLayers && !checkValidationLayerSupport()) 
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    #include "configs/device/appInfo.conf"
    VkInstanceCreateInfo createInfo = {};
    auto extensions = getRequiredExtensions();
    #include "configs/device/createInfo.conf"

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
          theGPU->physicalDevice = device;
          break;
      }
    }
  
    if (theGPU->physicalDevice == VK_NULL_HANDLE) 
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
  
    vkGetPhysicalDeviceProperties(theGPU->physicalDevice, &theGPU->deviceproperties);
    std::cout << "physical device: " << theGPU->deviceproperties.deviceName << std::endl;
}

static void createLogicalDevice(Device* theGPU) 
{
    QueueFamilyIndices indices = findQueueFamilies(theGPU->physicalDevice, theGPU);
  
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
  
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) 
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        #include "configs/device/queueCreateInfo.conf"
    }
  
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
  
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
  
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
  
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
  
    if (vkCreateDevice(theGPU->physicalDevice, &createInfo, nullptr, &theGPU->device_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
  
    vkGetDeviceQueue(theGPU->device_, indices.graphicsFamily, 0, &theGPU->graphicsQueue_);
    vkGetDeviceQueue(theGPU->device_, indices.presentFamily, 0, &theGPU->presentQueue_);
}

static void createCommandPool(Device* theGPU) 
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(theGPU->physicalDevice, theGPU);
  
    VkCommandPoolCreateInfo poolInfo = {};
    #include "configs/device/poolInfo.conf" 
    if (vkCreateCommandPool(theGPU->device_, &poolInfo, nullptr, &theGPU->commandPool) != VK_SUCCESS) 
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
        vkGetPhysicalDeviceFormatProperties(theGPU->physicalDevice, format, &props);

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
    vkGetPhysicalDeviceMemoryProperties(theGPU->physicalDevice, &memProperties);
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

void createDeviceBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer &buffer,
    VkDeviceMemory &bufferMemory,
    Device* theGPU) 
{
    VkBufferCreateInfo bufferInfo{};
    #include "configs/device/bufferCreateInfo.conf"
    if (vkCreateBuffer(theGPU->device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(theGPU->device_, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    #include "configs/device/memoryAllocateInfo.conf"
    if (vkAllocateMemory(theGPU->device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(theGPU->device_, buffer, bufferMemory, 0);
}

VkCommandBuffer beginSingleTimeCommands(Device* theGPU) 
{
    VkCommandBufferAllocateInfo allocInfo{};
    #include "configs/device/commandBufferAllocInfo.conf" 
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(theGPU->device_, &allocInfo, &commandBuffer);
  
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
  
    vkFreeCommandBuffers(theGPU->device_, theGPU->commandPool, 1, &commandBuffer);
}

void copyBuffer(Device* theGPU, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(theGPU);
  
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
  
    endSingleTimeCommands(commandBuffer, theGPU);
}

void copyBufferToImage(
        Device* theGPU,
    VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(theGPU);
  
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
  
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;
  
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};
  
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
    endSingleTimeCommands(commandBuffer, theGPU);
}

void createImageWithInfo(
    const VkImageCreateInfo &imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &imageMemory,
      Device* theGPU) 
{
    if (vkCreateImage(theGPU->device_, &imageInfo, nullptr, &image) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(theGPU->device_, image, &memRequirements);
  
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, theGPU);

    if (vkAllocateMemory(theGPU->device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }
  
    if (vkBindImageMemory(theGPU->device_, image, imageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }
}

} 


