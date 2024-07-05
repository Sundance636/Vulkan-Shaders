#include "Application.h"


Application::Application() {
    loadEntities();
}

Application::~Application() {

}


void Application::run() {
    RenderSystem RenderSystem{appDevice,appRenderer.getSwapChainRenderPass()};
    Camera camera{};
    camera.setViewDirection(glm::vec3{0.0f}, glm::vec3{0.5f,0.0f,1.0f});

    //stores camera info
    auto viewerObject = Entity::createEntity();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

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

        if(auto commandBuffer = appRenderer.beginFrame()) {
            appRenderer.beginSwapChainRenderPass(commandBuffer);
            RenderSystem.renderObjects(commandBuffer, entities, camera);

            appRenderer.endSwapChainRenderPass(commandBuffer);
            appRenderer.endFrame();
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

