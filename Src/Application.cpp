#include "Application.h"


//Uniform Buffer Object
struct  GlobalUbo {
    glm::mat4 projectionView{1.0f};
    alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f,-3.0f,1.0f});
    float deltaTime = 0;
};


Application::Application() {
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


    RenderSystem renderSystem{appDevice,appRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    Camera camera{};
    camera.setViewDirection(glm::vec3{0.0f}, glm::vec3{0.5f,0.0f,1.0f});

    //stores camera info
    auto viewerObject = Entity::createEntity();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = 0;
    float wavetime = 0;
    auto newTime2 = std::chrono::high_resolution_clock::now();

    while(!ApplicationWindow.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        std::cout << frameTime * 1000 << "ms\n";//convert to miliseconds

        cameraController.moveInPlaneXZ(ApplicationWindow.getGLFWwindow(),frameTime,viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspectRatio = appRenderer.getAspectRatio();

        //camera.setOrthographicProjection(-aspectRatio,aspectRatio,-1,1,-1,1);
        camera.setPerspectiveProjection(glm::two_pi<float>()/8.0f,aspectRatio,0.1f,50.0f);

        wavetime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - newTime2).count();

        if(wavetime >= 0.01f) {
            dt+= 0.01f;//keeps it so its independent of frame start/end
            newTime2 = std::chrono::high_resolution_clock::now();


        }


        if(auto commandBuffer = appRenderer.beginFrame()) {
            //prepping objects
            int frameindex = appRenderer.getFrameIndex();
            FrameInfo frameInfo {
                frameindex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameindex]
            };

            GlobalUbo ubo{};
            ubo.lightDirection = viewerObject.transform.translation;

            ubo.projectionView = camera.getProjection() * camera.getViewMat();
            ubo.deltaTime = dt;
            uboBuffers[frameindex]->writeToBuffer(&ubo);
            uboBuffers[frameindex]->flush();


            //render stage
            appRenderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderObjects(frameInfo, entities);

            appRenderer.endSwapChainRenderPass(commandBuffer);
            appRenderer.endFrame();
        }



    }
    
    vkDeviceWaitIdle(appDevice.device());
}


void Application::loadEntities() {
    std::shared_ptr<Model> appModel =  Model::createModelFromFile(appDevice,"Models/plane.obj");

    auto loadedObject = Entity::createEntity();
    loadedObject.model = appModel;
    //loadedObject.transform.translation = {0.0f,0.0f,20.5f};
    loadedObject.transform.scale = glm::vec3{1.5f};
    entities.push_back(std::move(loadedObject));

    
}

