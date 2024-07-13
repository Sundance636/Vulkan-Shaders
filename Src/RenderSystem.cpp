#include "RenderSystem.h"

struct SimplePushConstantData {
    float deltaTime = 0;
    alignas(16) glm::mat4 modelMatrix;
};

RenderSystem::RenderSystem(coreDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : appDevice{device}  {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);


}

RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(appDevice.device(), pipelineLayout, nullptr);
}

void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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

void RenderSystem::renderObjects(FrameInfo& frameInfo, std::vector<Entity>&Objects) {
    Pipeline->bind(frameInfo.commandBuffer);
    static float dt = 0.0f;

    //glm::mat4 projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getViewMat();

    vkCmdBindDescriptorSets(frameInfo.commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pipelineLayout,0,1,&frameInfo.globalDescriptorSet,0,nullptr);
        
    for( auto &obj : Objects) {
        SimplePushConstantData push{};



        push.modelMatrix = obj.transform.fastMat4();
        push.deltaTime = dt;
        //push.transform = projectionView * push.modelMatrix;

        //record to cmdbuffer
        vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
        obj.model->bind(frameInfo.commandBuffer);
        obj.model->draw(frameInfo.commandBuffer);
    }
    dt +=0.001f;
}
