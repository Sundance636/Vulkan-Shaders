#include "Application.h"


//Uniform Buffer Object
/*
    UBO is a type of Resource in a descriptor, and a generalized
    buffer containing abritrary info to 
    send to the shaders
*/
struct  GlobalUbo {
    glm::mat4 projectionView{1.0f};
    alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f,-3.0f,1.0f});
};


Application::Application() {
    
    //Descriptors Pools will contain many descriptor sets
    globalPool = DescriptorPool::Builder(appDevice)
    .setMaxSets(coreSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,coreSwapChain::MAX_FRAMES_IN_FLIGHT)
    .build();

    loadEntities();
}

Application::~Application() {

}


void Application::run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(coreSwapChain::MAX_FRAMES_IN_FLIGHT);


    //Allocating UBOs
    for (uint32_t i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
            appDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    auto globalSetLayout = DescriptorSetLayout::Builder(appDevice)
    .addBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
    .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(coreSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }



    //Preliminaries Before Rendering
    /*
        1. Need to allocate memory for a framebuffer to render to
        that matches the dimensions  and format of the swapchain
    */

    RenderSystem renderSystem{appDevice,offRenderer.getoffRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    

    Camera camera{};
    camera.setViewDirection(glm::vec3{0.0f}, glm::vec3{0.5f,0.0f,1.0f});

    //stores camera info
    auto viewerObject = Entity::createEntity();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();
    
    VkDescriptorPool globalPool2;
    VkDescriptorSetLayout glolayout;

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    uboLayoutBinding.descriptorCount = 1;

    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(appDevice.device(), &layoutInfo, nullptr, &glolayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }


    //Give a different renderpass?
    ComputeSystem computeSystem{appDevice,offRenderer.getoffRenderPass(),glolayout};

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSize.descriptorCount = static_cast<uint32_t>(1);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(1);



    if (vkCreateDescriptorPool(appDevice.device(), &poolInfo, nullptr, &globalPool2) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    VkDescriptorSetAllocateInfo compDescInfo{};
    compDescInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    compDescInfo.pNext = nullptr;
    compDescInfo.descriptorPool = globalPool2;
    compDescInfo.descriptorSetCount = 1;
    compDescInfo.pSetLayouts = &glolayout; 


    VkSemaphore waitSemaphore;
    VkSemaphore FinishedSemaphore;
    VkSemaphore imageAvailableSemaphore;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(appDevice.device(), &semaphoreInfo, nullptr, &waitSemaphore);
    vkCreateSemaphore(appDevice.device(), &semaphoreInfo, nullptr, &FinishedSemaphore);
    vkCreateSemaphore(appDevice.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);

    
    VkSemaphore signalSemaphores[] = {FinishedSemaphore};
    VkSwapchainKHR swapChains[] = {appRenderer.getSwapChain()};

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence commandBufferFence;
    vkCreateFence(appDevice.device(), &fenceInfo, nullptr, &commandBufferFence);


    VkDescriptorSet computeSet;
    vkAllocateDescriptorSets( appDevice.device(), &compDescInfo,&computeSet);

    while(!ApplicationWindow.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

       // std::cout << frameTime * 1000 << "ms\n";//convert to miliseconds

        cameraController.moveInPlaneXZ(ApplicationWindow.getGLFWwindow(),frameTime,viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspectRatio = appRenderer.getAspectRatio();

        //camera.setOrthographicProjection(-aspectRatio,aspectRatio,-1,1,-1,1);
        camera.setPerspectiveProjection(glm::two_pi<float>()/8.0f,aspectRatio,0.1f,50.0f);



        //checks if frame can be started(i.e the the command buffer done processing?)
        if(auto commandBuffer = offRenderer.beginFrame()) {
            //prepping objects
            int frameindex = 0;
            uint32_t index = frameindex;


            //FIX: THIS LINE CAUSES CRASH
            vkAcquireNextImageKHR(appDevice.device(), swapChains[0], UINT64_MAX, waitSemaphore, VK_NULL_HANDLE, &index);
            uint32_t aquiredIndex = index;
            index = index%coreSwapChain::MAX_FRAMES_IN_FLIGHT;
            //std::cout << "aquire index: " << index << "\n";

            FrameInfo frameInfo {
                aquiredIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[index]
            };

            GlobalUbo ubo{};
            ubo.lightDirection = viewerObject.transform.translation;

            ubo.projectionView = camera.getProjection() * camera.getViewMat();
            uboBuffers[index]->writeToBuffer(&ubo);
            uboBuffers[index]->flush();

            offRenderer.transitionImgLayout(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            //Render stage (Currently Presenting to the Swapchain)
            offRenderer.beginOffRenderPass(commandBuffer);
            renderSystem.renderObjects(frameInfo, entities);
            offRenderer.endOffRenderPass(commandBuffer);
            

            //transition the imagelayout so its readable by compute shader
            //perform layout transitions using image memory barriers
            offRenderer.transitionImgLayout(commandBuffer,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,VK_IMAGE_LAYOUT_GENERAL);


            //be sure to create and bind the correct descriptors (offscreen image resource)
            offRenderer.bindDescriptors(computeSet);

            //Post processing effects (From off screen render)
            computeSystem.computeCall(frameInfo,computeSet);
            
            
            //copy anti aliased image to swap chain for presentation TODO
            appRenderer.transitionImgLayout(commandBuffer,VK_IMAGE_LAYOUT_UNDEFINED ,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aquiredIndex );

            //copying
            offRenderer.copyImgtoSwapchain(commandBuffer,appRenderer, aquiredIndex);

            //transition copied swapchain image
            appRenderer.transitionImgLayout(commandBuffer,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, aquiredIndex);

            offRenderer.endFrame();

            //present to swapchain
            //appRenderer.submitBuffers(commandBuffer,frameindex);


            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &waitSemaphore;
            submitInfo.pWaitDstStageMask = waitStages;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;


            

            vkQueueSubmit(appDevice.graphicsQueue(), 1, &submitInfo, commandBufferFence);

            vkWaitForFences(appDevice.device(), 1, &commandBufferFence, VK_TRUE, UINT64_MAX);
            vkResetFences(appDevice.device(), 1, &commandBufferFence);

            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &aquiredIndex;


            //std::cout << "SFAS\n";
            vkQueuePresentKHR(appDevice.presentQueue(), &presentInfo);


        }

    }

    
    vkDeviceWaitIdle(appDevice.device());
}


void Application::loadEntities() {
    std::shared_ptr<Model> appModel =  Model::createModelFromFile(appDevice,"Models/Sora2.obj");

    auto loadedObject = Entity::createEntity();
    loadedObject.model = appModel;
    loadedObject.transform.translation = {0.0f,0.0f,2.5f};
    loadedObject.transform.scale = glm::vec3{1.0f};
    entities.push_back(std::move(loadedObject));

    
}

