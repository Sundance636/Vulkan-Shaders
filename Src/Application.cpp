#include "Application.h"

struct SimplePushConstantData {
    glm::mat2 transform{1.0f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};


Application::Application() {
    loadEntities();
    createPipelineLayout();
    createPipeline();


}

Application::~Application() {
    vkDestroyPipelineLayout(appDevice.device(), pipelineLayout, nullptr);
}


void Application::run() {
    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();

        if(auto commandBuffer = appRenderer.beginFrame()) {
            appRenderer.beginSwapChainRenderPass(commandBuffer);
            renderObjects(commandBuffer);
            appRenderer.endSwapChainRenderPass(commandBuffer);
            appRenderer.endFrame();
        }

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


    pipelineConfig.renderPass = appRenderer.getSwapChainRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    Pipeline = std::make_unique<pipeline>(appDevice,"shaders/vert.spv","shaders/frag.spv",pipelineConfig);


}

void Application::renderObjects(VkCommandBuffer commandBuffer) {
    Pipeline->bind(commandBuffer);
    static float rotation = 0;
    rotation += 0.1f;


    for( auto &obj : entities) {
        SimplePushConstantData push{};
        obj.transform2d.rotation = rotation;
        push.offset = obj.transform2d.translation;
        push.color = obj.color;
        push.transform = obj.transform2d.mat2();

        //record to cmdbuffer
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}

void Application::loadEntities() {
    //vertex position, then colotr, which is thne interpolated
    std::vector<Model::Vertex> vertices {
        {{0.0f, -0.5f},{1.0f,0.0f,0.0f}},
        {{0.5f, 0.5f},{0.0f,1.0f,0.0f}},
        {{-0.5f, 0.5f},{0.0f,0.0f,1.0f}}
    };


    auto appModel = std::make_shared<Model>(appDevice, vertices);
    Entity triangle = Entity::createEntity();
    triangle.model = appModel;
    triangle.color = {0.8f, 0.8f, 0.8f};
    //triangle.transform2d.translation.x = 0.2f;


    //triangle.transform2d.scale = {2.0f, 0.5f };

    entities.push_back(std::move(triangle));
}