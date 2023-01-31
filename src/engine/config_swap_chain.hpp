#pragma once

#include "swap_chain.hpp"

namespace config
{
VkSwapchainCreateInfoKHR createSwapchainInfo(bve::SwapChain *swapchain, bve::SwapChainSupportDetails &swapChainSupport, VkSurfaceFormatKHR &surfaceFormat, VkPresentModeKHR &presentMode, VkExtent2D &extent, uint32_t imageCount);


VkImageViewCreateInfo createImageViewInfo(bve::SwapChain *swapchain, size_t swapchainImageIndex);

}
