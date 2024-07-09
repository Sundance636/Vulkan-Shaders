#include "Application.h"


Application::Application() {
    //loadEntities();
    loadVectorField();
}

Application::~Application() {

}


void Application::run() {
    RenderSystem Rendersystem{appDevice,appRenderer.getSwapChainRenderPass()};

    //second render system for vector Field
    //VectorRenderSystem Rendersystem2{appDevice,appRenderer.getSwapChainRenderPass()};


    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();

        if(auto commandBuffer = appRenderer.beginFrame()) {
            appRenderer.beginSwapChainRenderPass(commandBuffer);
            Rendersystem.renderObjects(commandBuffer, entities);

            //Rendersystem2.renderObjects(commandBuffer,  Vfield);

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
        {{0.5f, 0.5f,},{0.0f,1.0f,0.0f}},
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

std::unique_ptr<Model> createSquareModel(coreDevice& device, glm::vec2 offset) {
  std::vector<Model::Vertex> vertices = {
      {{-0.5f, -0.5f}},
      {{0.5f, 0.5f}},
      {{-0.5f, 0.5f}},
      {{-0.5f, -0.5f}},
      {{0.5f, -0.5f}},
      {{0.5f, 0.5f}},  //
  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<Model>(device, vertices);
}

//initialize vector field here
void Application::loadVectorField() {
    //vertex position, then colotr, which is thne interpolated

    std::shared_ptr<Model>  appModel = createSquareModel( appDevice, {0.5f, 0.0f});


  int gridCount = 40;
  for (int i = 0; i < gridCount; i++) {
    for (int j = 0; j < gridCount; j++) {
      auto vf = Entity::createEntity();
      vf.transform2d.scale = glm::vec3(0.005f);
      vf.transform2d.translation = {
          -1.0f + (i + 0.5f) * 2.0f / gridCount,
          -1.0f + (j + 0.5f) * 2.0f / gridCount};
      vf.color = glm::vec3(1.0f);
      vf.model = appModel;
      entities.push_back(std::move(vf));
    }
  }

    
}