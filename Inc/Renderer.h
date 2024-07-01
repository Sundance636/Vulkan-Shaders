#ifndef __Renderer_h__
#define __Renderer_h__

#include "viewPort.h"
#include "swapChain.h"
#include "model.h"

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

class Renderer {

    private:
        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted;

        viewPort& RendererWindow;// = viewPort(winWIDTH,winHEIGHT,"Vulkan Renderer");

        coreDevice& appDevice;// = coreDevice(RendererWindow);
        std::unique_ptr<coreSwapChain> SwapChain;// = coreSwapChain(appDevice, RendererWindow.getExtent());
        std::vector<VkCommandBuffer> commandBuffers;

        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

    public:
        Renderer(viewPort& window, coreDevice& device);
        ~Renderer();

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        bool isFrameInProgress() const;
        VkCommandBuffer getCurrentCommandBuffer() const;
        int getFrameIndex() const;

        VkRenderPass getSwapChainRenderPass() const;// { return SwapChain->getRenderPass(); }

        Renderer(const Renderer&) = delete;
        Renderer &operator=(const Renderer&) = delete;
        
};

#endif