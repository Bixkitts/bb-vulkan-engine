#pragma once

#include "bve_device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace bve 
{

 //public:
    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    struct BveSwapChain
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

        BveDevice *device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
        
    };

    BveSwapChain *createBveSwapChain(BveDevice *device, VkExtent2D windowExtent);
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
    float extentAspectRatio(BveSwapChain *swapchain); 
    
    VkFormat findDepthFormat(BveSwapChain *swapchain);

    VkResult acquireNextImage(BveSwapChain *swapchain, uint32_t *imageIndex);
    VkResult submitCommandBuffers(BveSwapChain *swapchain, const VkCommandBuffer *buffers, uint32_t *imageIndex);

 //private:
    static void initSwapChain(BveSwapChain *swapchain);
    static void createImageViews(BveSwapChain *swapchain);
    static void createDepthResources(BveSwapChain *swapchain);
    static void createRenderPass(BveSwapChain *swapchain);
    static void createFramebuffers(BveSwapChain *swapchain);
    static void createSyncObjects(BveSwapChain *swapchain);

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(BveSwapChain *swapchain, const VkSurfaceCapabilitiesKHR &capabilities);

}    // namespace lve
