#include "Renderer.h"


Renderer::Renderer( viewPort &window, coreDevice& device) : RendererWindow{window}, appDevice{device} {
    isFrameStarted = false;
    currentImageIndex = 0;
    currentFrameIndex = 0;
    recreateSwapChain();
    createCommandBuffers();

}

Renderer::~Renderer() {
    freeCommandBuffers();
}

bool Renderer::isFrameInProgress() const { 
    return isFrameStarted; 
}

VkCommandBuffer Renderer::getCurrentCommandBuffer() const { 
    assert( isFrameStarted && "Cannot get command buffer when frame not in progress!");
    return commandBuffers[currentFrameIndex];
}

VkRenderPass Renderer::getSwapChainRenderPass() const { 
    return SwapChain->getRenderPass(); 
}

float Renderer::getAspectRatio() const {
    return SwapChain->extentAspectRatio();
}

int Renderer::getFrameIndex() const {
    assert( isFrameStarted && "Cannot get frame index when frame not in progress!");
    return currentFrameIndex;
}


void Renderer::createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo{};

    commandBuffers.resize(coreSwapChain::MAX_FRAMES_IN_FLIGHT);
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//primary may be subimted for execution, not called by other command buffers
    allocInfo.commandPool = appDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

     if (vkAllocateCommandBuffers(appDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

}

void Renderer::freeCommandBuffers() {
    vkFreeCommandBuffers(appDevice.device(), appDevice.getCommandPool(),  static_cast<uint32_t>(commandBuffers.size()),commandBuffers.data() );
    commandBuffers.clear();

}

void Renderer::recreateSwapChain() {
    VkExtent2D extent = RendererWindow.getExtent();


    while (extent.width == 0 || extent.height == 0) {
        extent = RendererWindow.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(appDevice.device());

    if(SwapChain == nullptr) {
            SwapChain = std::make_unique<coreSwapChain>(appDevice, extent);

    } else {
        std::shared_ptr<coreSwapChain> oldSwapChain = std::move(SwapChain);
        SwapChain = std::make_unique<coreSwapChain>(appDevice, extent, oldSwapChain);

        if(!oldSwapChain->compareSwapFormats(*SwapChain.get())) {
            throw std::runtime_error("Swapchain image(or depth) format has changed!");
            //make callback later
        }

    }

}

VkCommandBuffer Renderer::beginFrame() {
    assert(!isFrameStarted && "Can't call beginFrame while already in progress");

    VkResult result = SwapChain->acquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        
        recreateSwapChain();
        return nullptr;
    }

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;
    VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void Renderer::endFrame() {
    assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

    VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || RendererWindow.wasframeBufferResized()) {
        RendererWindow.resetWindowResizedFlag();
        recreateSwapChain();
    } else if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % coreSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin renderpass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = SwapChain->getRenderPass();
    renderPassInfo.framebuffer = SwapChain->getFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

    std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(SwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(SwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, SwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't end renderpass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);

}