#include "viewPort.h"

viewPort::viewPort() {
    WIDTH = 640;
    HEIGHT = 480;
    windowName = "Default";

    initWindow();
}

viewPort::viewPort( u_int32_t Width, uint32_t Height, std::string name) {
    this->WIDTH = Width;
    this->HEIGHT = Height;
    this->windowName = name;

    initWindow();
}

viewPort::~viewPort() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void viewPort::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);
    //glfwSetWindowUserPointer(window, this);

    //glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool viewPort::shouldClose() {
    return glfwWindowShouldClose(window);
}

void viewPort::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}