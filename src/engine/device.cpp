#include "device.hpp"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vulkan/vulkan_core.h>

// TODO: some global variables??? std::vectors??? fix this maybe.
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
VulkanExtensions deviceExtensions;

static void                    createVulkanInstance              (Device theGPU);
static void                    setupDebugMessenger               (Device theGPU);
static void                    createSurface                     (Device theGPU);
static void                    pickPhysicalDevice                (Device theGPU);
static void                    createLogicalDevice               (Device theGPU);
static void                    createCommandPool                 (Device theGPU);

static bool                    isDeviceSuitable                  (const VkPhysicalDevice device, 
                                                                  const Device theGPU);
static bool                    checkDeviceExtensionSupport       (VkPhysicalDevice device);
static bool                    checkValidationLayerSupport       ();
static void                    populateDebugMessengerCreateInfo  (VkDebugUtilsMessengerCreateInfoEXT &createInfo);
static void                    hasGflwRequiredInstanceExtensions ();
static BBError                 getRequiredInstanceExtensions     (VulkanExtensions *extensions);

// Config info creator functions
static inline void                        createAppInfo               (VkApplicationInfo *appInfo);
static inline VkCommandPoolCreateInfo     createCommandPoolCreateInfo (const QueueFamilyIndices queueFamilyIndices);
static inline VkCommandBufferAllocateInfo commandBufferAllocInfo      (Device theGPU);
static inline void                        createQueueCreateInfo       (VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                                                                       uint32_t queueFamily, 
                                                                       const float *queuePriority );
static inline void                        createInstanceCreateInfo    (VkInstanceCreateInfo *createInfo,
                                                                       const VkApplicationInfo *appInfo, 
                                                                       const VulkanExtensions *extensions);
static inline void                        createDeviceCreateInfo      (VkDeviceCreateInfo *createInfo,
                                                                       const VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                                                                       const VkPhysicalDeviceFeatures *deviceFeatures, 
                                                                       const VulkanExtensions *deviceExtensions);
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
    createVulkanInstance      (*device);
    setupDebugMessenger (*device); 
    createWindowSurface ((*device)->window, 
                         (*device)->instance, 
                         &(*device)->surface_);
    pickPhysicalDevice  (*device);
    createLogicalDevice (*device);
    createCommandPool   (*device);
    return BB_ERROR_OK;
}

void destroyDevice(Device *device) 
{
    vkDestroyCommandPool ((*device)->logical, 
                          (*device)->commandPool, 
                          NULL);
    if (enableValidationLayers){
        DestroyDebugUtilsMessengerEXT((*device)->instance, 
                                      (*device)->debugMessenger, 
                                      NULL);
    }
    vkDestroySurfaceKHR  ((*device)->instance, 
                          (*device)->surface_, 
                          NULL);
    vkDestroyInstance    ((*device)->instance, 
                          NULL);
    vkDeviceWaitIdle     ((*device)->logical);
    vkDestroyDevice      ((*device)->logical, 
                          NULL);
    free                 ((*device));
    *device = NULL;
}

static void createVulkanInstance(Device theGPU) 
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
  
    for (const auto &device : devices) {
        if (isDeviceSuitable(device, theGPU)) {
          theGPU->physical = device;
          break;
      }
    }
  
    if (theGPU->physical == VK_NULL_HANDLE) {
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
  
    //std::set<std::string> requiredExtensions(deviceExtensions.extensions[0], deviceExtensions.extensions[deviceExtensions.count-1]);
 // 
 //   for (const auto &extension : availableExtensions) {
 //       requiredExtensions.erase(extension.extensionName);
 //   }
 // 
 //   return requiredExtensions.empty();
    return 0;
}

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice device, const Device theGPU) 
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

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const Device theGPU) 
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

VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, 
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
uint32_t findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties, const Device theGPU) 
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

static void createAppInfo(VkApplicationInfo *appInfo)
{
    *appInfo                    = {};
    appInfo->sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo->pApplicationName   = "VulkanEngine App";
    appInfo->applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo->pEngineName        = "No Engine";
    appInfo->engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo->apiVersion         = VK_API_VERSION_1_0;
}
static void createInstanceCreateInfo(VkInstanceCreateInfo *createInfo,
                                     const VkApplicationInfo *appInfo, 
                                     const VulkanExtensions *extensions)
{
    *createInfo                         = {};
    createInfo->sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo->pApplicationInfo        = appInfo;
    createInfo->enabledExtensionCount   = extensions->count;
    createInfo->ppEnabledExtensionNames = extensions->extensions;
}
static void createQueueCreateInfo(VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                                  const uint32_t queueFamily, 
                                  const float *queuePriority)
{
    queueCreateInfos->createInfos[0].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos->createInfos[0].queueFamilyIndex = queueFamily;
    queueCreateInfos->createInfos[0].queueCount       = 1;
    queueCreateInfos->createInfos[0].pQueuePriorities = queuePriority;

    queueCreateInfos->queueCount                      = 1;
}
static VkCommandPoolCreateInfo createCommandPoolCreateInfo(const QueueFamilyIndices queueFamilyIndices)
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags            =
    VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 
    | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    return poolInfo;
}
static void createDeviceCreateInfo(VkDeviceCreateInfo *createInfo,
                                   const VkDeviceQueueCreateInfoArray *queueCreateInfos, 
                                   const VkPhysicalDeviceFeatures *deviceFeatures, 
                                   const VulkanExtensions *deviceExtensions)
{
    *createInfo = {};
    createInfo->sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo->queueCreateInfoCount    = queueCreateInfos->queueCount;
    createInfo->pQueueCreateInfos       = queueCreateInfos->createInfos;
    createInfo->pEnabledFeatures        = deviceFeatures;
    createInfo->enabledExtensionCount   = deviceExtensions->count;
    createInfo->ppEnabledExtensionNames = deviceExtensions->extensions;
}
