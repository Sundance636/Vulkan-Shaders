#include "Application.h"


Application::Application() {
    loadEntities();
    loadVectorField();
}

Application::~Application() {

}


void Application::run() {
    RenderSystem Rendersystem{appDevice,appRenderer.getSwapChainRenderPass()};

    //second render system for vector Field
    VectorRenderSystem Rendersystem2{appDevice,appRenderer.getSwapChainRenderPass()};


    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();

        if(auto commandBuffer = appRenderer.beginFrame()) {
            appRenderer.beginSwapChainRenderPass(commandBuffer);
            Rendersystem.renderObjects(commandBuffer, entities);

            Rendersystem2.renderObjects(commandBuffer,  Vfield);

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

//initialize vector field here
void Application::loadVectorField() {
    //vertex position, then colotr, which is thne interpolated
    


    //triangle.transform2d.scale = {2.0f, 0.5f };
    for(float i = -0.9f; i < 1.0f; i += 0.1f ) {
        for(float j = -0.9f; j < 1.0f; j += 0.1f ) {

        std::vector<Model::Vertex> vertices {
            {{i, j},{1.0f,0.0f,0.0f}},
            {{i+0.01f, j},{0.0f,1.0f,0.0f}},
            {{i, j},{0.0f,0.0f,1.0f}}
        };


        auto appModel = std::make_shared<Model>(appDevice, vertices);
        VectorField field = VectorField::createVectorField();
        field.model = appModel;
        field.color = {0.8f, 0.8f, 0.8f};
        //triangle.transform2d.translation.x = 0.2f;


            Vfield.push_back(std::move(field));
        }
            
    }

    
}