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

}


void Application::drawFrame() {

}