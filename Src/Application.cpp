#include "Application.h"


Application::Application() {
    //loadEntities();
    loadVectorField();
}

Application::~Application() {

}

std::unique_ptr<Model> createCircleModel(coreDevice& device, unsigned int numSides);

void Application::run() {
    RenderSystem Rendersystem{appDevice,appRenderer.getSwapChainRenderPass()};

    PhysicsSystem physicsEngine{9.81};//init to earths gravity

    //second render system for vector Field
    //VectorRenderSystem Rendersystem2{appDevice,appRenderer.getSwapChainRenderPass()};

    std::shared_ptr<Model> circleModel = createCircleModel(appDevice, 64);

    // create physics objects
    std::vector<Entity> physicsObjects{};
    Entity red = Entity::createEntity();
    red.transform2d.scale = glm::vec3{.05f};
    //red.transform2d.translation = {.5f, .5f};
    red.color = {1.f, 0.f, 0.f};
    red.bodyComponent.velocity = {-.5f, .0f};

    red.model = circleModel;
    physicsObjects.push_back(std::move(red));



    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();

        if(auto commandBuffer = appRenderer.beginFrame()) {

            physicsEngine.ComputeField(entities,physicsObjects);

            appRenderer.beginSwapChainRenderPass(commandBuffer);


            Rendersystem.renderObjects(commandBuffer, physicsObjects);

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


std::unique_ptr<Model> createCircleModel(coreDevice& device, unsigned int numSides) {
  std::vector<Model::Vertex> uniqueVertices{};
  for (uint32_t i = 0; i < numSides; i++) {
    float angle = i * glm::two_pi<float>() / numSides;
    uniqueVertices.push_back({{glm::cos(angle), glm::sin(angle)}});
  }
  uniqueVertices.push_back({});  // adds center vertex at 0, 0

  std::vector<Model::Vertex> vertices{};
  for (uint32_t i = 0; i < numSides; i++) {
    vertices.push_back(uniqueVertices[i]);
    vertices.push_back(uniqueVertices[(i + 1) % numSides]);
    vertices.push_back(uniqueVertices[numSides]);
  }
  return std::make_unique<Model>(device, vertices);
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
      vf.color = glm::vec3(1.0f * i/gridCount);
      vf.model = appModel;
      entities.push_back(std::move(vf));
    }
  }

    
}