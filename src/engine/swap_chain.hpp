#ifndef SWAP_CHAIN
#define SWAP_CHAIN

#include <vector>
#include <vulkan/vulkan.h>
#include "defines.hpp"
#include "device.hpp"

typedef struct SwapChain_T SwapChain_T;
BB_OPAQUE_HANDLE(SwapChain);

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
