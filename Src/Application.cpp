#include "Application.h"

Application::Application() {


}

void Application::run() {
    while (!ApplicationWindow.shouldClose()) {
        glfwPollEvents();
        //drawFrame();
    }

}