#include "RenderSystem.h"

struct SimplePushConstantData {
    glm::mat4 transform{1.0f};
    alignas(16) glm::mat4 modelMatrix;
};

RenderSystem::RenderSystem(coreDevice& device, VkRenderPass renderPass) : appDevice{device}  {
    createPipelineLayout();
    createPipeline(renderPass);


}

RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(appDevice.device(), pipelineLayout, nullptr);
}

void RenderSystem::createPipelineLayout() {

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

void RenderSystem::createPipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    
    //PipelineConfigInfo pipelineConfig = pipeline::defaultPipelineConfigInfo(SwapChain->width(),SwapChain->height());
    pipeline::defaultPipelineConfigInfo(pipelineConfig);


    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    Pipeline = std::make_unique<pipeline>(appDevice,"shaders/vert.spv","shaders/frag.spv",pipelineConfig);


}

void RenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<Entity>&Objects, const Camera &camera) {
    Pipeline->bind(commandBuffer);

    glm::mat4 projectionView = camera.getProjection() * camera.getViewMat();


    for( auto &obj : Objects) {
        SimplePushConstantData push{};



        push.modelMatrix = obj.transform.fastMat4();
        push.transform = projectionView * push.modelMatrix;

        //record to cmdbuffer
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}
