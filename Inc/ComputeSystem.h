#ifndef __ComputeSystem_h__
#define __ComputeSystem_h__

#include "pipeline.h"
#include "model.h"
#include "entity.h"
#include "Camera.h"
#include "FrameInfo.h"

//forces radians across all platforms
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //maybe change later
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <memory>
#include <iostream>

class ComputeSystem {

    private:

        coreDevice& appDevice;

        std::unique_ptr<pipeline> Pipeline;
        VkPipelineLayout pipelineLayout;


        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

    public:
        ComputeSystem(coreDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~ComputeSystem();

        void computeCall(FrameInfo& frameInfo);


        ComputeSystem(const ComputeSystem&) = delete;
        ComputeSystem &operator=(const ComputeSystem&) = delete;
        
};

#endif