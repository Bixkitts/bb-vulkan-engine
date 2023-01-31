#include "draw_frame.hpp"
#include "buffers.hpp"
#include "swap_chain.hpp"
#include "command_buffers.hpp"
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <chrono>

namespace bve
{

void drawFrame(SwapChain* swapchain, std::vector<VkCommandBuffer> &commandBuffers, std::vector<UniformBuffer*> &uniformBuffers)
{
    uint32_t imageIndex;
    auto result = acquireNextImage(swapchain, &imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(imageIndex, swapchain, uniformBuffers);

    result = submitCommandBuffers(swapchain, &commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to run command from command buffer!");
    }

}

void updateUniformBuffer(uint32_t currentImage, SwapChain *swapchain, std::vector<UniformBuffer*> &uniformBuffers)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    Matrices ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapchain->swapChainExtent.width / (float) swapchain->swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffers[currentImage]->mapped, &ubo, sizeof(ubo));

}

}
