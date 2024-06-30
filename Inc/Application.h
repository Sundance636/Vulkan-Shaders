#ifndef __Application_h__
#define __Application_h__

#include "viewPort.h"
#include "pipeline.h"
#include "swapChain.h"
#include "model.h"
#include "entity.h"

//forces radians across all platforms
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //maybe change later
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <memory>
#include <iostream>

class Application {

    private:

        //viewPort Specifications
        uint32_t winWIDTH = 640;
        uint32_t winHEIGHT = 480;
        viewPort ApplicationWindow = viewPort(winWIDTH,winHEIGHT,"Vulkan Renderer");

        coreDevice appDevice = coreDevice(ApplicationWindow);
        std::unique_ptr<coreSwapChain> SwapChain;// = coreSwapChain(appDevice, ApplicationWindow.getExtent());
        std::unique_ptr<pipeline> Pipeline;
        //pipeline Pipeline = pipeline(appDevice,"shaders/vert.spv","shaders/frag.spv",pipeline::defaultPipelineConfigInfo(winWIDTH,winHEIGHT));
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<Entity> entities;

        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommanfBuffers();
        void drawFrame();
        void loadEntities();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void renderObjects(VkCommandBuffer commandBuffer);

    public:
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application &operator=(const Application&) = delete;
        

        void run();

};

#endif