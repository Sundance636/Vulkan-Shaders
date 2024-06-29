#include "Application.h"

Application::Application() {
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();

}

Application::~Application() {
    vkDestroyPipelineLayout(appDevice.device(), pipelineLayout, nullptr);
}


void Application::run() {
    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();
        drawFrame();
        vkDeviceWaitIdle(appDevice.device());
    }

}

void Application::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    
    if (vkCreatePipelineLayout(appDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
    }
}

void Application::createPipeline() {
    auto pipelineConfig = pipeline::defaultPipelineConfigInfo(SwapChain.width(),SwapChain.height());

    pipelineConfig.renderPass = SwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    Pipeline = std::make_unique<pipeline>(appDevice,"shaders/vert.spv","shaders/frag.spv",pipelineConfig);


}


void Application::createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo{};

    commandBuffers.resize(SwapChain.imageCount());
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//primary may be subimted for execution, not called by other command buffers
    allocInfo.commandPool = appDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

     if (vkAllocateCommandBuffers(appDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for( int i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = SwapChain.getRenderPass();
        renderPassInfo.framebuffer = SwapChain.getFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = SwapChain.getSwapChainExtent();

        std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        Pipeline->bind(commandBuffers[i]);
        //3,1,0,0 draws three vertices in one instance, 0,0 for no offsets
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

    }

}


void Application::drawFrame() {
    uint32_t imageIndex;

    auto result = SwapChain.acquireNextImage(&imageIndex);

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    result = SwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

}