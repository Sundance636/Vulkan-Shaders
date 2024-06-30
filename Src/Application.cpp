#include "Application.h"

struct SimplePushConstantData {
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};


Application::Application() {
    loadModels();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();

}

Application::~Application() {
    vkDestroyPipelineLayout(appDevice.device(), pipelineLayout, nullptr);
}


void Application::run() {
    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();
        drawFrame();
    }
    
    vkDeviceWaitIdle(appDevice.device());
}

void Application::createPipelineLayout() {

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);


    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    
    if (vkCreatePipelineLayout(appDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
    }
}

void Application::createPipeline() {
    PipelineConfigInfo pipelineConfig{};
    
    //PipelineConfigInfo pipelineConfig = pipeline::defaultPipelineConfigInfo(SwapChain->width(),SwapChain->height());
    pipeline::defaultPipelineConfigInfo(pipelineConfig);


    pipelineConfig.renderPass = SwapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    Pipeline = std::make_unique<pipeline>(appDevice,"shaders/vert.spv","shaders/frag.spv",pipelineConfig);


}


void Application::createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo{};

    commandBuffers.resize(SwapChain->imageCount());
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//primary may be subimted for execution, not called by other command buffers
    allocInfo.commandPool = appDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

     if (vkAllocateCommandBuffers(appDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

}

void Application::freeCommanfBuffers() {
    vkFreeCommandBuffers(appDevice.device(), appDevice.getCommandPool(),  static_cast<uint32_t>(commandBuffers.size()),commandBuffers.data() );
    commandBuffers.clear();

}

void Application::recordCommandBuffer(int imageIndex) {
    static int frame = 0;
    frame = (frame +1) % 1000;
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = SwapChain->getRenderPass();
    renderPassInfo.framebuffer = SwapChain->getFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

    std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(SwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(SwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, SwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
    
    
    Pipeline->bind(commandBuffers[imageIndex]);

    appModel->bind(commandBuffers[imageIndex]);

    //draws 4 times / 4 copies of object
    for(int j = 0; j < 4; j++) {
        SimplePushConstantData push{};
        push.offset = {-0.5f + frame*0.002f, -0.4f + j * 0.25f };
        push.color = {0.0f, 0.0f, 0.2f + 0.2f *j};

        //record to cmdbuffer
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
        appModel->draw(commandBuffers[imageIndex]);

    }

    

    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Application::drawFrame() {
    uint32_t imageIndex;
    VkResult result = SwapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        
        recreateSwapChain();
        return;
    }

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    recordCommandBuffer(imageIndex);
    result = SwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || ApplicationWindow.wasframeBufferResized()) {
        ApplicationWindow.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }
    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

}

void Application::recreateSwapChain() {
    VkExtent2D extent = ApplicationWindow.getExtent();


    while (extent.width == 0 || extent.height == 0) {
        extent = ApplicationWindow.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(appDevice.device());

    if(SwapChain == nullptr) {
            SwapChain = std::make_unique<coreSwapChain>(appDevice, extent);

    } else {
        SwapChain = std::make_unique<coreSwapChain>(appDevice, extent, std::move(SwapChain));
        if(SwapChain->imageCount() != commandBuffers.size()) {
            freeCommanfBuffers();
            createCommandBuffers();
        }
    }

    //check for render pass compatibility then no need to recreate pipeline
    createPipeline();

}

void Application::loadModels() {
    //vertex position, then colotr, which is thne interpolated
    std::vector<Model::Vertex> vertices {
        {{0.0f, -0.5f},{1.0f,0.0f,0.0f}},
        {{0.5f, 0.5f},{0.0f,1.0f,0.0f}},
        {{-0.5f, 0.5f},{0.0f,0.0f,1.0f}}
    };


    appModel = std::make_unique<Model>(appDevice, vertices);
}

