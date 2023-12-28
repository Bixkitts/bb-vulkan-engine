#include "device.hpp"
#include "config_device.hpp"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vulkan/vulkan_core.h>

// local callback functions
static VKAPI_ATTR VkBool32 VKAPI_CALL 
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) 
{
    //TODO: stdlib shit
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                             const VkAllocationCallbacks* pAllocator,
                                             VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
    // TODO: C++ gibberish
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } 
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
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

BBError deviceInit(Device *device, BBWindow deviceWindow) 
{
    // TODO: MALLOC without free()
    *device = (Device)calloc(1, sizeof(Device_T));
    if(*device == NULL){
        return BB_ERROR_MEM;
    }
    (*device)->window = deviceWindow;
    createInstance      (*device);
    setupDebugMessenger (*device); 
    createWindowSurface ((*device)->window, 
                         (*device)->instance, 
                         &(*device)->surface_);
    pickPhysicalDevice  (*device);
    createLogicalDevice (*device);
    createCommandPool   (*device);
    return BB_ERROR_OK;
}

void destroyDevice(Device device) 
{
    vkDestroyCommandPool (device->logical, 
                          device->commandPool, 
                          NULL);
    if (enableValidationLayers){
        DestroyDebugUtilsMessengerEXT(device->instance, 
                                      device->debugMessenger, 
                                      NULL);
    }
    vkDestroySurfaceKHR  (device->instance, 
                          device->surface_, 
                          NULL);
    vkDestroyInstance    (device->instance, 
                          NULL);
    vkDeviceWaitIdle     (device->logical);
    vkDestroyDevice      (device->logical, 
                          NULL);
    free                 (device);
}

static void createInstance(Device theGPU) 
{
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo                  appInfo;
    VkInstanceCreateInfo               createInfo;
    VulkanExtensions                   requiredExtensions;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

    createAppInfo                 (&appInfo);
    getRequiredInstanceExtensions (&requiredExtensions);
    createInstanceCreateInfo      (&createInfo, 
                                   &appInfo, 
                                   &requiredExtensions);

    if (enableValidationLayers){
        createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT* )&debugCreateInfo;
    } 
    else{
        createInfo.enabledLayerCount = 0;
        createInfo.pNext             = NULL;
    }

    if (vkCreateInstance(&createInfo, NULL, &theGPU->instance) != VK_SUCCESS){
        throw std::runtime_error("failed to create instance!");
    }

    hasGflwRequiredInstanceExtensions();
}

static void pickPhysicalDevice(Device theGPU) 
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

static void createLogicalDevice(Device theGPU) 
{
    QueueFamilyIndices           indices             = findQueueFamilies(theGPU->physical, 
                                                                         theGPU);
    VkDeviceCreateInfo           createInfo;
    VkDeviceQueueCreateInfoArray queueCreateInfos;
    std::set<uint32_t>           uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
    float                        queuePriority       = 1.0f;
    VkDeviceQueueCreateInfo      queueCreateInfo;
    VkPhysicalDeviceFeatures     deviceFeatures      = {};

    deviceFeatures.samplerAnisotropy = VK_TRUE;
  
    for (uint32_t queueFamily : uniqueQueueFamilies){
        createQueueCreateInfo(&queueCreateInfos, 
                              queueFamily, 
                              &queuePriority);
    }
    createDeviceCreateInfo(&createInfo, &queueCreateInfos, &deviceFeatures, &deviceExtensions);

    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
    if (enableValidationLayers) {
        createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } 
    else{
        createInfo.enabledLayerCount   = 0;
    }
 
    if (vkCreateDevice(theGPU->physical, 
                       &createInfo, 
                       NULL, 
                       &theGPU->logical) 
        != VK_SUCCESS){
        throw std::runtime_error("failed to create logical device!");
    }
  
    vkGetDeviceQueue(theGPU->logical, indices.graphicsFamily, 0, &theGPU->graphicsQueue_);
    vkGetDeviceQueue(theGPU->logical, indices.presentFamily, 0, &theGPU->presentQueue_);
}

static void createCommandPool(Device theGPU) 
{
    QueueFamilyIndices      queueFamilyIndices = findQueueFamilies           (theGPU->physical, theGPU);
    VkCommandPoolCreateInfo poolInfo           = createCommandPoolCreateInfo (queueFamilyIndices);

    if (vkCreateCommandPool(theGPU->logical, 
                            &poolInfo, 
                            NULL, 
                            &theGPU->commandPool) 
        != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

static bool isDeviceSuitable(const VkPhysicalDevice device, const Device theGPU) 
{
    QueueFamilyIndices       indices            = findQueueFamilies(device, theGPU);
    VkPhysicalDeviceFeatures supportedFeatures  = {};
    SwapChainSupportDetails  swapChainSupport   = {};

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate   = false;

    if (extensionsSupported) {
        swapChainSupport  = querySwapChainSupport(device, theGPU);
        swapChainAdequate = !swapChainSupport.formats.empty() 
                            && !swapChainSupport.presentModes.empty();
    }
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    return indices.isComplete() 
           && extensionsSupported 
           && swapChainAdequate 
           && supportedFeatures.samplerAnisotropy;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) 
{
    createInfo = {};

    createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData       = NULL;  // Optional
}

static void setupDebugMessenger(Device theGPU)
{
    if (!enableValidationLayers){
        return;
    }
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(theGPU->instance, 
                                     &createInfo, 
                                     nullptr, 
                                     &theGPU->debugMessenger) 
        != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

static bool checkValidationLayerSupport() 
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
    }
    if (!layerFound) {
        return false;
    }
  }

  return true;
}

static BBError getRequiredInstanceExtensions(VulkanExtensions *extensions)  
{
    // TODO: Fuck all of this, get rid of GLFW.
    //---------------------------------------------
    const int    headroom = 20; // How much extra memory we want to allocate off the end of the buffer
                                // in case we want to manually insert extensions :/
    const char **temp     = glfwGetRequiredInstanceExtensions(&extensions->count);
    // TODO: MALLOC without free
    const char **temp2    = (const char**)calloc(extensions->count+headroom, sizeof(VkExtensionProperties));
    if (temp2 == NULL) {
        return BB_ERROR_MEM;
    }
    for (int i = 0; i < extensions->count; i++){
        temp2[i] = temp[i];
    }
    free(temp);
    extensions->extensions = temp2;
    if (enableValidationLayers) {
        extensions->extensions[extensions->count+1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        extensions->count++;
    }
    return BB_ERROR_OK;
}  

static void hasGflwRequiredInstanceExtensions(void) 
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    // TODO: watch out for stack overflow
    VkExtensionProperties availableExtensions[extensionCount];
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions);
  
    // TODO: stdlib shit
    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> available;
    for (int i = 0; i < extensionCount; i++) {
        std::cout << "\t" << availableExtensions[i].extensionName << std::endl;
        available.insert(availableExtensions[i].extensionName);
    }
  
    std::cout << "required extensions:" << std::endl;
    VulkanExtensions requiredExtensions;
    getRequiredInstanceExtensions(&requiredExtensions);
    for (int i = 0; i < requiredExtensions.count; i++)  
    {
        std::cout << "\t" << requiredExtensions.extensions[i] << std::endl;
        if (available.find(requiredExtensions.extensions[i]) == available.end()) {
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

static bool checkDeviceExtensionSupport(VkPhysicalDevice device) 
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device,
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

static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice device, const Device theGPU) 
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
            indices.graphicsFamily         = i;
            indices.graphicsFamilyHasValue = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, theGPU->surface_, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily         = i;
            indices.presentFamilyHasValue = true;
        }
        if (indices.isComplete()) {
            break;
        }
    
        i++;
    }
  
    return indices;
}

static SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const Device theGPU) 
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

static VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, 
                                    const VkImageTiling tiling, 
                                    const VkFormatFeatureFlags features,
                                    const Device theGPU) 
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

//Query the physical device for the types of memory available to allocate
static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Device theGPU) 
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(theGPU->physical, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) 
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
  
    throw std::runtime_error("failed to find suitable memory type!");
}
