#ifndef __RenderSystem_h__
#define __RenderSystem_h__

#include "pipeline.h"
#include "model.h"
#include "entity.h"

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

class RenderSystem {

    private:

        coreDevice& appDevice;

        std::unique_ptr<pipeline> Pipeline;
        VkPipelineLayout pipelineLayout;

        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

    public:
        RenderSystem(coreDevice& device, VkRenderPass renderPass);
        ~RenderSystem();

        void renderObjects(VkCommandBuffer commandBuffer, std::vector<Entity>&Objects);


        RenderSystem(const RenderSystem&) = delete;
        RenderSystem &operator=(const RenderSystem&) = delete;
        
};

#endif