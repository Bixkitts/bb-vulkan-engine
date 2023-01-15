#include "config_swap_chain.hpp"
#include "bve_device.hpp"
#include "bve_swap_chain.hpp"
#include <vulkan/vulkan_core.h>

namespace config
{
VkSwapchainCreateInfoKHR createSwapchainInfo(bve::SwapChain *swapchain, bve::SwapChainSupportDetails &swapChainSupport, VkSurfaceFormatKHR &surfaceFormat, VkPresentModeKHR &presentMode, VkExtent2D &extent, uint32_t imageCount)
{

    VkSwapchainCreateInfoKHR createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = swapchain->device->surface_;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    bve::QueueFamilyIndices indices = findQueueFamilies(swapchain->device->physical, swapchain->device);

    auto *queueFamilyIndices = new uint32_t[2];
    queueFamilyIndices[0]=indices.graphicsFamily;
    queueFamilyIndices[1]=indices.presentFamily;

    if (indices.graphicsFamily != indices.presentFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } 
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;            // Optional
        createInfo.pQueueFamilyIndices = nullptr;    // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;


    return createInfo;

}
VkImageViewCreateInfo createImageViewInfo(bve::SwapChain *swapchain, size_t swapchainImageIndex)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = swapchain->swapChainImages[swapchainImageIndex];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapchain->swapChainImageFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    return viewInfo;

}

}
