#include <vulkan/vulkan_core.h>
#include "bve_device.hpp"
#include "config_device.hpp"

namespace config
{
    VkApplicationInfo appInfo()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VulkanEngine App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        return appInfo;
    }
    VkInstanceCreateInfo instanceCreateInfo(VkApplicationInfo &appInfo, std::vector<const char*> &extensions )
    {
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        return createInfo;
    }
    VkDeviceQueueCreateInfo queueCreateInfo(std::vector<VkDeviceQueueCreateInfo> &queueCreateInfos, uint32_t &queueFamily, float &queuePriority )
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
        return queueCreateInfo;
    }
    VkMemoryAllocateInfo memoryAllocateInfo(VkMemoryRequirements &memRequirements,VkMemoryPropertyFlags &properties, bve::Device *theGPU)
    {
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = bve::findMemoryType(memRequirements.memoryTypeBits, properties, theGPU);
        return allocInfo;
    }
    VkCommandPoolCreateInfo poolInfo(bve::QueueFamilyIndices &queueFamilyIndices)
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags =
                VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        return poolInfo;
    }
    VkCommandBufferAllocateInfo commandBufferAllocInfo(bve::Device *theGPU)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = theGPU->commandPool;
        allocInfo.commandBufferCount = 1;
        return allocInfo;
    }
    VkBufferCreateInfo bufferCreateInfo(VkDeviceSize &size, VkBufferUsageFlags &usage)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        return bufferInfo;
    }
}
