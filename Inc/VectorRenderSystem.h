#ifndef __VectorRenderSystem_h__
#define __VectorRenderSystem_h__

#include "pipeline.h"
#include "model.h"
#include "entity.h"
#include "VectorField.h"

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

class VectorRenderSystem {

    private:

        coreDevice& appDevice;

        std::unique_ptr<pipeline> Pipeline;
        VkPipelineLayout pipelineLayout;

        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

    public:
        VectorRenderSystem(coreDevice& device, VkRenderPass renderPass);
        ~VectorRenderSystem();

        void renderObjects(VkCommandBuffer commandBuffer, std::vector<VectorField>&Objects);


        VectorRenderSystem(const VectorRenderSystem&) = delete;
        VectorRenderSystem &operator=(const VectorRenderSystem&) = delete;
        
};

#endif