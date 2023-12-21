#ifndef SWAP_CHAIN
#define SWAP_CHAIN

#include <vector>
#include <vulkan/vulkan.h>
#include "device.hpp"

typedef struct SwapChain_T
{
    //TODO: stdlib shit
    Device                      device;
    std::vector<VkFramebuffer>  framebuffers;
    VkRenderPass                renderPass;
    std::vector<VkImage>        depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView>    depthImageViews;
    std::vector<VkImage>        swapChainImages;
    std::vector<VkImageView>    swapChainImageViews;
    VkExtent2D                  windowExtent;
    VkFormat                    swapChainImageFormat;
    VkExtent2D                  swapChainExtent;
    VkSwapchainKHR              swapChain;
    std::vector<VkSemaphore>    imageAvailableSemaphores;
    std::vector<VkSemaphore>    renderFinishedSemaphores;
    std::vector<VkFence>        inFlightFences;
    size_t                      currentFrame;
    
}SwapChain_T, *SwapChain;

BBError            createSwapChain         (SwapChain *swapchain, 
                                            const Device device, 
                                            const VkExtent2D windowExtent);
void               destroySwapchain        (SwapChain swapchain);
float              getExtentAspectRatio    (SwapChain swapchain); 

VkFormat           findDepthFormat         (SwapChain swapchain);

VkResult           acquireNextImage        (SwapChain swapchain, 
                                            uint32_t *imageIndex);
// TODO: stdlib shit, and returns
VkSurfaceFormatKHR chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR> &availableFormats);
VkPresentModeKHR   chooseSwapPresentMode   (const std::vector<VkPresentModeKHR> &availablePresentModes);
VkExtent2D         chooseSwapExtent        (SwapChain swapchain, 
                                            const VkSurfaceCapabilitiesKHR capabilities);

#endif
