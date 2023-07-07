#pragma once

#include "device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace bve 
{

 //public:
    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    struct SwapChain
    {
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        Device *device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        size_t currentFrame = 0;
        
    };

    SwapChain *createSwapChain(Device *device, VkExtent2D windowExtent);
    void destroySwapchain(SwapChain* swapchain);
/*
    VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() { return renderPass; }
    VkImageView getImageView(int index) { return swapChainImageViews[index]; }
    size_t imageCount() { return swapChainImages.size(); }
    VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return swapChainExtent; }
    uint32_t width() { return swapChainExtent.width; }
    uint32_t height() { return swapChainExtent.height; }
*/
    float extentAspectRatio(SwapChain *swapchain); 
    
    VkFormat findDepthFormat(SwapChain *swapchain);

    VkResult acquireNextImage(SwapChain *swapchain, uint32_t *imageIndex);

 //private:
    static void initSwapChain(SwapChain *swapchain);
    static void createSwapchainImageViews(SwapChain *swapchain);
    static void createDepthResources(SwapChain *swapchain);
    static void createRenderPass(SwapChain *swapchain);
    static void createFramebuffers(SwapChain *swapchain);
    static void createSyncObjects(SwapChain *swapchain);

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(SwapChain *swapchain, const VkSurfaceCapabilitiesKHR &capabilities);

}    
