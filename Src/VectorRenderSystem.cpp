#include "VectorRenderSystem.h"

struct SimplePushConstantData {
    glm::mat3 transform{1.0f};
    glm::vec2 offset;
    alignas(32) glm::vec3 color;
};

VectorRenderSystem::VectorRenderSystem(coreDevice& device, VkRenderPass renderPass) : appDevice{device}  {
    createPipelineLayout();
    createPipeline(renderPass);


}

VectorRenderSystem::~VectorRenderSystem() {
    vkDestroyPipelineLayout(appDevice.device(), pipelineLayout, nullptr);
}

void VectorRenderSystem::createPipelineLayout() {

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData) ;


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

void VectorRenderSystem::createPipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    
    //PipelineConfigInfo pipelineConfig = pipeline::defaultPipelineConfigInfo(SwapChain->width(),SwapChain->height());
    pipeline::defaultPipelineConfigInfo(pipelineConfig);


    //define this pipline to construct lines
    pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;


    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    Pipeline = std::make_unique<pipeline>(appDevice,"shaders/vert.spv","shaders/frag.spv",pipelineConfig);


}

void VectorRenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<VectorField>&Objects) {
    Pipeline->bind(commandBuffer);
    static float rotation = 0;
    rotation += 0.01f;


    for( auto &obj : Objects) {
        SimplePushConstantData push{};
        obj.transform2d.rotation = rotation;
        //obj.transform2d.translation = {obj.getPosition()};
        push.offset = obj.transform2d.translation;
        push.color = obj.color;
        push.transform = obj.transform2d.mat3();

        //record to cmdbuffer
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}
