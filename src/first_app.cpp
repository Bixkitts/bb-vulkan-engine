#include "first_app.hpp"
#include "GLFW/glfw3.h"
#include "bve_command_buffers.hpp"
#include "bve_device.hpp"
#include "bve_draw_frame.hpp"
#include "bve_model.hpp"
#include "bve_pipeline.hpp"
#include "bve_swap_chain.hpp"
#include "bve_window.hpp"
#include <vulkan/vulkan_core.h>

#include <vector>
#include <array>

namespace bve
{
    void runAppWithWindow(BveWindow *mainWindow)
    {
        BveDevice *device = bveDeviceInit(mainWindow);

        BveSwapChain *swapchain = createBveSwapChain(device, getExtent(mainWindow));

        std::vector<BveModel*> models = loadModels(device);

        PipelineConfigInfo *pipelineConfig = defaultPipelineConfigInfo(swapchain->swapChainExtent.width,swapchain->swapChainExtent.height);


        //Extra config steps that should get sorted
        pipelineConfig->renderPass = swapchain->renderPass;
        pipelineConfig->pipelineLayout = createPipelineLayout(device);
        //-----------------------------------------
        //creating the pipe line
        BveGraphicsPipeline *pipeline = 
            createBveGraphicsPipeline(device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", pipelineConfig);

        //Command buffer section
        std::vector<VkCommandBuffer> commandBuffers;
        createCommandBuffers(pipeline, commandBuffers,swapchain, models);

        //--------------------------------------------
        
        while (!glfwWindowShouldClose(mainWindow->window))
        {
            glfwPollEvents();
            drawFrame(swapchain, commandBuffers); 
        }

        vkDeviceWaitIdle(device->device_);
    }
}
