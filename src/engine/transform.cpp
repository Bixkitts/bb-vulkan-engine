#include "transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

// TODO: a whole bunch. Make this a proper transformation
#include <glm/glm.hpp>
#include <chrono>

void updateUniformBuffers(SwapChain swapchain, UniformBuffer uniformBuffers[])
{
    PerObjectMatrices ubo         = {};
    static auto       startTime   = std::chrono::high_resolution_clock::now();
    auto              currentTime = std::chrono::high_resolution_clock::now();
    float             time        = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ubo.model = glm::rotate      (glm::mat4(1.0f), 
                                  time * glm::radians(90.0f), 
                                  glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view  = glm::lookAt      (glm::vec3(2.0f, 2.0f, 2.0f), 
                                  glm::vec3(0.0f, 0.0f, 0.0f), 
                                  glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj  = glm::perspective (glm::radians(45.0f), 
                                  swapchain->swapChainExtent.width / 
                                  (float) swapchain->swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    copyIntoMappedMem(uniformBuffers[swapchain->currentFrame], 
                      &ubo, 
                      sizeof(PerObjectMatrices));
}
