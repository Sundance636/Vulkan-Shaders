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

        cameraController.moveInPlaneXZ(ApplicationWindow.getGLFWwindow(),frameTime,viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspectRatio = appRenderer.getAspectRatio();

        //camera.setOrthographicProjection(-aspectRatio,aspectRatio,-1,1,-1,1);
        camera.setPerspectiveProjection(glm::two_pi<float>()/8.0f,aspectRatio,0.1f,10.0f);

        if(auto commandBuffer = appRenderer.beginFrame()) {
            appRenderer.beginSwapChainRenderPass(commandBuffer);
            RenderSystem.renderObjects(commandBuffer, entities, camera);

            appRenderer.endSwapChainRenderPass(commandBuffer);
            appRenderer.endFrame();
        }

    }
    
    vkDeviceWaitIdle(appDevice.device());
}

std::unique_ptr<Model> createCubeModel(coreDevice& device, glm::vec3 offset) {

    Model::Builder modelBuilder{};

    modelBuilder.vertices = {
      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
 
      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
 
      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
 
      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
 
      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
 
      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
  };
  for (auto& v : modelBuilder.vertices) {
    v.position += offset;
  }
 
  modelBuilder.indicies = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                          12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
 

  return std::make_unique<Model>(device, modelBuilder);
}


void Application::loadEntities() {
    std::shared_ptr<Model> appModel = createCubeModel(appDevice,{0.0f,0.0f,0.0f});

    auto cube = Entity::createEntity();
    cube.model = appModel;
    cube.transform.translation = {0.0f,0.0f,2.5f};
    cube.transform.scale = {0.5f,0.5f,0.5f};
    entities.push_back(std::move(cube));
}

