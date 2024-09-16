#ifndef __Renderer_h__
#define __Renderer_h__

#include "viewPort.h"
#include "swapChain.h"
#include "model.h"
//#include "OffScreenRenderer.h"

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

        VkRenderPass offScreenRenderPass;
       // std::unique_ptr<OffScreenRenderer> offScreen;
        

        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();
        void createOffscreenRenderpass();

    public:
        Renderer(viewPort& window, coreDevice& device);
        ~Renderer();

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        void createOffscreenFramebuffer(VkDevice device, VkExtent2D extent);


        bool isFrameInProgress() const;
        VkCommandBuffer getCurrentCommandBuffer() const;
        int getFrameIndex() const;

        VkRenderPass getSwapChainRenderPass() const;// { return SwapChain->getRenderPass(); }
        float getAspectRatio() const;

        void transitionImgLayout(VkCommandBuffer& cmdBuffer,VkImageLayout oldLayout,VkImageLayout newLayout,uint32_t index);

        VkImage getSwapChainImage(uint32_t index);
        
        VkSwapchainKHR getSwapChain();
        void submitBuffers(VkCommandBuffer& commandBuffers,uint32_t index);
        

        Renderer(const Renderer&) = delete;
        Renderer &operator=(const Renderer&) = delete;
        
};

#endif