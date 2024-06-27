#ifndef __pipeline_h__
#define __pipeline_h__

#include "coreDevice.h"

#include <string>
#include <fstream>
#include <vector>

struct PipelineConfigInfo {

};

//defining the graphics pipeline
class pipeline {

    private:
        coreDevice* Device;//pointer to deevice make sure to free
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;

        
        void createGraphicsPipeline(const std::string& vertShaderFile, const std::string& fragShaderFile, const PipelineConfigInfo& configInfo);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
        
        static std::vector<char> readFile(const std::string& filename);

    public:
        pipeline();
        pipeline(coreDevice& device, const std::string& vertShaderFile, const std::string& fragShaderFile, const PipelineConfigInfo& configInfo);
        ~pipeline();

        pipeline(const pipeline&) = delete;
        void operator=(const pipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        
    
};



#endif