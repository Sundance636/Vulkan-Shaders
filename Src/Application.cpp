#include "Application.h"


Application::Application() {
    loadEntities();
}

Application::~Application() {

}


void Application::run() {
    RenderSystem RenderSystem{appDevice,appRenderer.getSwapChainRenderPass()};

    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();

        if(auto commandBuffer = appRenderer.beginFrame()) {
            appRenderer.beginSwapChainRenderPass(commandBuffer);
            RenderSystem.renderObjects(commandBuffer, entities);

            appRenderer.endSwapChainRenderPass(commandBuffer);
            appRenderer.endFrame();
        }

    }
    
    vkDeviceWaitIdle(appDevice.device());
}


void Application::loadEntities() {
    //vertex position, then colotr, which is thne interpolated
    std::vector<Model::Vertex> vertices {
        {{0.0f, -0.5f},{1.0f,0.0f,0.0f}},
        {{0.5f, 0.5f},{0.0f,1.0f,0.0f}},
        {{-0.5f, 0.5f},{0.0f,0.0f,1.0f}}
    };


    auto appModel = std::make_shared<Model>(appDevice, vertices);
    Entity triangle = Entity::createEntity();
    triangle.model = appModel;
    triangle.color = {0.8f, 0.8f, 0.8f};
    //triangle.transform2d.translation.x = 0.2f;


    //triangle.transform2d.scale = {2.0f, 0.5f };

    entities.push_back(std::move(triangle));
}