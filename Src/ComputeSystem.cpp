#include "ComputeSystem.h"

struct SimplePushConstantData {
    glm::mat4 transform{1.0f};
    alignas(16) glm::mat4 modelMatrix;
};

ComputeSystem::ComputeSystem(coreDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : appDevice{device}  {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);


}

ComputeSystem::~ComputeSystem() {
    vkDestroyPipelineLayout(appDevice.device(), pipelineLayout, nullptr);
}

void ComputeSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();


    //uncomment later to use push constants with pipeline
    //pipelineLayoutInfo.pushConstantRangeCount = 1;
    //pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    
    if (vkCreatePipelineLayout(appDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
    }
}

void ComputeSystem::createPipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    

    //call may cause issues
    pipeline::defaultPipelineConfigInfo(pipelineConfig);


    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    Pipeline = std::make_unique<pipeline>(appDevice,"shaders/comp.spv",pipelineConfig);


}

void ComputeSystem::computeCall(FrameInfo& frameInfo,VkDescriptorSet& computeDesc) {
    Pipeline->bindCompute(frameInfo.commandBuffer);
    
    vkCmdBindDescriptorSets(frameInfo.commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE ,pipelineLayout,0,1,&computeDesc,0,nullptr);
    //vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(SimplePushConstantData), &push);

    vkCmdDispatch(frameInfo.commandBuffer,64,1,1);

}
