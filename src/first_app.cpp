#include "first_app.hpp"
#include "GLFW/glfw3.h"
#include "bve_device.hpp"
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include "bve_window.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>

namespace bve
{
    void runAppWithWindow(BveWindow *mainWindow)
    {
        BveDevice *device = bveDeviceInit(mainWindow);

        BveSwapChain *swapchain = createBveSwapChain(device, getExtent(mainWindow));

        PipelineConfigInfo *pipelineConfig = defaultPipelineConfigInfo(swapchain->swapChainExtent.width,swapchain->swapChainExtent.height);


        std::vector<VkCommandBuffer> commandBuffers;

        //Extra config steps that should get sorted
        pipelineConfig->renderPass = swapchain->renderPass;
        pipelineConfig->pipelineLayout = createPipelineLayout(device);



        //-----------------------------------------
        BveGraphicsPipeline *pipeline = 
            createBveGraphicsPipeline(device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", pipelineConfig);


        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
        }
    }
}
