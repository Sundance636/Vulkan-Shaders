#include "pipeline.h"


pipeline::pipeline() {

}


pipeline::pipeline(coreDevice& device, const std::string& vertShaderFile, const std::string& fragShaderFile, const PipelineConfigInfo& configInfo) { 
    createGraphicsPipeline(vertShaderFile, fragShaderFile, configInfo);
}

pipeline::~pipeline() {

}


void pipeline::createGraphicsPipeline(const std::string& vertShaderFile, const std::string& fragShaderFile, const PipelineConfigInfo& configInfo) {
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");
}

void pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule( Device->device() , &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

}

PipelineConfigInfo pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo configInfo{};


    return configInfo;
}



std::vector<char> pipeline::readFile(const std::string& filename) {
    //ate seeks to end to get file size easily
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}