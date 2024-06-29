#ifndef __Application_h__
#define __Application_h__

#include "viewPort.h"
#include "pipeline.h"
#include "swapChain.h"
#include "model.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <memory>

class Application {

    private:


        //viewPort Specifications
        uint32_t winWIDTH = 640;
        uint32_t winHEIGHT = 480;
        viewPort ApplicationWindow = viewPort(winWIDTH,winHEIGHT,"Vulkan Renderer");

        coreDevice appDevice = coreDevice(ApplicationWindow);
        coreSwapChain SwapChain = coreSwapChain(appDevice, ApplicationWindow.getExtent());
        std::unique_ptr<pipeline> Pipeline;
        //pipeline Pipeline = pipeline(appDevice,"shaders/vert.spv","shaders/frag.spv",pipeline::defaultPipelineConfigInfo(winWIDTH,winHEIGHT));
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<Model> appModel;

        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
        void loadModels();


    public:
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application &operator=(const Application&) = delete;
        

        void run();

};

#endif