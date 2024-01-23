#include "device.hpp"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vulkan/vulkan_core.h>

struct Device_T {
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
};
// TODO: some global variables??? std::vectors??? fix this maybe.
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
static const bool enableValidationLayers = false;
#else
static const bool enableValidationLayers = true;
#endif

static void    createVulkanInstance              (Device_T *device, const VulkanExtensions *requiredExtensions);
static void    setupDebugMessenger               (Device_T *device);
static void    createSurface                     (Device_T *device);
static void    pickPhysicalDevice                (Device_T *device, const VulkanExtensions *requiredExtensions);
static void    createLogicalDevice               (Device_T *device, const VulkanExtensions *requiredExtensions);
static void    createCommandPool                 (Device_T *device);

static bool    isDeviceSuitable                  (Device_T *device, 
                                                  const VulkanExtensions *requiredExtensions);
static bool    checkDeviceExtensionSupport       (const VkPhysicalDevice device, 
                                                  const VulkanExtensions *requiredExtensions);
static bool    checkValidationLayerSupport       ();
static void    populateDebugMessengerCreateInfo  (VkDebugUtilsMessengerCreateInfoEXT *createInfo);
static void    hasGflwRequiredInstanceExtensions ();
static BBError allocateVulkanExtensions          (VulkanExtensions *extensions);
static BBError getRequiredDeviceExtensions       (VulkanExtensions *extensions);
static BBError getRequiredInstanceExtensions     (VulkanExtensions *extensions);

// Config info creator functions
static inline void                        createAppInfo               (VkApplicationInfo *appInfo);
static inline VkCommandPoolCreateInfo     createCommandPoolCreateInfo (const QueueFamilyIndices queueFamilyIndices);
static inline VkCommandBufferAllocateInfo commandBufferAllocInfo      (Device_T *device);
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

BBError createDevice(Device *device, const BBWindow deviceWindow) 
{
    VulkanExtensions requiredDeviceExtensions   = {0};
    VulkanExtensions requiredInstanceExtensions = {0};
    // TODO: MALLOC without free()
    *device = (Device)calloc(1, sizeof(Device_T));
    if(*device == NULL){
        return BB_ERROR_MEM;
    }
    (*device)->window = deviceWindow;
    getRequiredInstanceExtensions (&requiredInstanceExtensions);
    createVulkanInstance          (*device, &requiredInstanceExtensions);
    setupDebugMessenger           (*device); 
    createWindowSurface           ((*device)->window, 
                                   (*device)->instance, 
                                   &(*device)->surface_);
    getRequiredDeviceExtensions   (&requiredDeviceExtensions);
    pickPhysicalDevice            (*device, &requiredDeviceExtensions);
    createLogicalDevice           (*device, &requiredDeviceExtensions);
    createCommandPool             (*device);
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

static void createVulkanInstance(Device_T *device, const VulkanExtensions *requiredExtensions) 
{
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo                  appInfo            = {};
    VkInstanceCreateInfo               createInfo         = {};
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo    = {};

    createAppInfo                 (&appInfo);
    createInstanceCreateInfo      (&createInfo, 
                                   &appInfo, 
                                   requiredExtensions);

    if (enableValidationLayers){
        createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT* )&debugCreateInfo;
    } 
    else{
        createInfo.enabledLayerCount   = 0;
        createInfo.pNext               = NULL;
    }
    if (vkCreateInstance(&createInfo, NULL, &device->instance) != VK_SUCCESS){
        fprintf(stderr, "failed to create vulkan instance.\n");
        exit(1);
    }

    hasGflwRequiredInstanceExtensions();
}

static void pickPhysicalDevice(Device_T *device, const VulkanExtensions *requiredExtensions) 
{
    uint32_t deviceCount = 0;
    // This function is run twice: first to get the number of devices,
    // and then again to actually get the device data
    vkEnumeratePhysicalDevices(device->instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        fprintf(stderr, "Failed to find GPUs with Vulkan support!\n");
        exit(1);
    }
    std::cout << "Device count: " << deviceCount << std::endl;
    VkPhysicalDevice potentialDevices[deviceCount];
    vkEnumeratePhysicalDevices(device->instance, &deviceCount, potentialDevices);
  
    for (int i = 0; i < deviceCount; i++) {
        device->physical = potentialDevices[i];
        if (isDeviceSuitable(device, requiredExtensions)) {
            break;
        }
    }
    if (device->physical == VK_NULL_HANDLE) {
        fprintf(stderr, "Failed to find a suitable GPU!\n");
        exit(1);
    }
  
    vkGetPhysicalDeviceProperties(device->physical, &device->physicalProperties);
    printf("Physical device: %s\n", device->physicalProperties.deviceName);
}

static void createLogicalDevice(Device_T *device, const VulkanExtensions *requiredDeviceExtensions) 
{
    QueueFamilyIndices           indices             = findQueueFamilies(device);
    VkDeviceCreateInfo           createInfo          = {};
    VkDeviceQueueCreateInfoArray queueCreateInfos    = {};
    std::set<uint32_t>           uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
    float                        queuePriority       = 1.0f;
    VkDeviceQueueCreateInfo      queueCreateInfo     = {};
    VkPhysicalDeviceFeatures     deviceFeatures      = {};

    deviceFeatures.samplerAnisotropy = VK_TRUE;
  
    for (uint32_t queueFamily : uniqueQueueFamilies){
        createQueueCreateInfo(&queueCreateInfos, 
                              queueFamily, 
                              &queuePriority);
    }
    createDeviceCreateInfo(&createInfo, 
                           &queueCreateInfos, 
                           &deviceFeatures, 
                           requiredDeviceExtensions);
    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
    if (enableValidationLayers) {
        createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } 
    else{
        createInfo.enabledLayerCount   = 0;
    }
 
    if (vkCreateDevice(device->physical, 
                       &createInfo, 
                       NULL, 
                       &device->logical) 
        != VK_SUCCESS){
        throw std::runtime_error("failed to create logical device!");
    }
  
    vkGetDeviceQueue(device->logical, indices.graphicsFamily, 0, &device->graphicsQueue_);
    vkGetDeviceQueue(device->logical, indices.presentFamily, 0, &device->presentQueue_);
}

static void createCommandPool(Device_T *device) 
{
    QueueFamilyIndices      queueFamilyIndices = findQueueFamilies           (device);
    VkCommandPoolCreateInfo poolInfo           = createCommandPoolCreateInfo (queueFamilyIndices);

    if (vkCreateCommandPool(device->logical, 
                            &poolInfo, 
                            NULL, 
                            &device->commandPool) 
        != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

static bool isDeviceSuitable(Device_T *device, const VulkanExtensions *requiredDeviceExtensions) 
{
    QueueFamilyIndices       indices                  = findQueueFamilies(device);
    VkPhysicalDeviceFeatures supportedFeatures        = {};
    SwapChainSupportDetails  swapChainSupport         = {};

    bool extensionsSupported = checkDeviceExtensionSupport(device->physical, requiredDeviceExtensions);
    bool swapChainAdequate   = false;

    if (extensionsSupported) {
        swapChainSupport  = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() 
                            && !swapChainSupport.presentModes.empty();
    }
    vkGetPhysicalDeviceFeatures(device->physical, &supportedFeatures);
    return indices.isComplete() 
           && extensionsSupported 
           && swapChainAdequate 
           && supportedFeatures.samplerAnisotropy;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo) 
{
    *createInfo = {};

    (*createInfo).sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    (*createInfo).messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    (*createInfo).messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    (*createInfo).pfnUserCallback = debugCallback;
    (*createInfo).pUserData       = NULL;  // Optional
}

static void setupDebugMessenger(Device_T *device)
{
    if (!enableValidationLayers){
        return;
    }
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(&createInfo);
    if (CreateDebugUtilsMessengerEXT(device->instance, 
                                     &createInfo, 
                                     nullptr, 
                                     &device->debugMessenger) 
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

static void deleteVulkanExtensions(VulkanExtensions *extensions)
{
   free(extensions->extensions[0]); 
   free(extensions->extensions); 
   extensions->extensions = NULL;
   extensions->count = 0;
}

static BBError allocateVulkanExtensions(VulkanExtensions *extensions)
{
    // If the memory is uninitialised this will crash :3
    if (extensions->extensions != NULL){
        deleteVulkanExtensions(extensions);
    }
    extensions->count = 0;

    extensions->extensions = (char**)calloc(MAX_EXTENSIONS, sizeof(char*));
    if (extensions->extensions == NULL) {
        return BB_ERROR_MEM;
    }
    char* buffer = (char*)calloc(MAX_EXTENSIONS*VK_MAX_EXTENSION_NAME_SIZE, sizeof(char));
    if (buffer == NULL) {
        return BB_ERROR_MEM;
    }

    for (int i = 0; i < MAX_EXTENSIONS; i++) {
        extensions->extensions[i] = buffer + (i * VK_MAX_EXTENSION_NAME_SIZE);
    }
    return BB_ERROR_OK;
}
static BBError getRequiredDeviceExtensions (VulkanExtensions *extensions)
{
    // enable device extensions here manually!
    // TODO: load from external file?
    allocateVulkanExtensions (extensions);
    strcpy                   (extensions->extensions[0], "VK_KHR_swapchain");
    extensions->count = 1;
    return BB_ERROR_OK;
}
static BBError getRequiredInstanceExtensions(VulkanExtensions *extensions)  
{
    allocateVulkanExtensions(extensions);
    const char **glfwGeneratedExtensionList = glfwGetRequiredInstanceExtensions(&extensions->count);

    for (int i = 0; i < extensions->count && i < MAX_EXTENSIONS; i++){
        strcpy(extensions->extensions[i], glfwGeneratedExtensionList[i]);
    }
    if (enableValidationLayers && extensions->count < MAX_EXTENSIONS) {
        strcpy(extensions->extensions[extensions->count], VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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

static bool checkDeviceExtensionSupport(const VkPhysicalDevice device, const VulkanExtensions *requiredExtensions) 
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
    return 1;
}

QueueFamilyIndices findQueueFamilies(Device_T *device) 
{
    QueueFamilyIndices indices;
  
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queueFamilyCount, nullptr);
  
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queueFamilyCount, queueFamilies.data());
  
    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily         = i;
            indices.graphicsFamilyHasValue = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device->physical, i, device->surface_, &presentSupport);
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

// All the get functions here
VkDevice getLogicalDevice(Device device)
{
    return device->logical;
}
VkCommandPool getDevCommandPool(Device device)
{
    return device->commandPool;
}
VkQueue getDevGraphicsQueue(Device device)
{
    return device->graphicsQueue_;
}
VkQueue getDevPresentQueue (Device device)
{
    return device->presentQueue_;
}
VkSurfaceKHR getDevVkSurface(Device device)
{
    return device->surface_;
}
VkPhysicalDeviceProperties getDevPhysicalProperties(Device device)
{
    return device->physicalProperties;
}


SwapChainSupportDetails querySwapChainSupport(Device_T *device) 
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physical, device->surface_, &details.capabilities);
  
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical, device->surface_, &formatCount, nullptr);
  
    if (formatCount != 0) 
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical, device->surface_, &formatCount, details.formats.data());
    }
  
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical, device->surface_, &presentModeCount, nullptr);
  
    if (presentModeCount != 0) 
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device->physical,
            device->surface_,
            &presentModeCount,
            details.presentModes.data());
    }
    return details;
}

VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, 
                             const VkImageTiling tiling, 
                             const VkFormatFeatureFlags features,
                             const Device device) 
{
    for (VkFormat format : candidates) 
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device->physical, format, &props);

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
uint32_t findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties, const Device device) 
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device->physical, &memProperties);
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
    createInfo->pNext                   = NULL;
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
