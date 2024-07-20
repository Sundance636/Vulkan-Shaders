#include "viewPort.h"

viewPort::viewPort() {
    WIDTH = 640;
    HEIGHT = 480;
    windowName = "Default";

    initWindow();
}

viewPort::viewPort( uint32_t Width, uint32_t Height, std::string name) {
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);
    
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool viewPort::shouldClose() {
    return glfwWindowShouldClose(window);
}

VkExtent2D viewPort::getExtent() {
    VkExtent2D extent {};

    //int width;
    //int height;
    //glfwGetFramebufferSize(window, &width, &height);


    extent.height = HEIGHT;
    extent.width = WIDTH;
    return extent;
}

GLFWwindow* viewPort::getGLFWwindow() const {
    return window;
}

bool viewPort::wasframeBufferResized() {
    return frameBufferResized;
}

void viewPort::resetWindowResizedFlag() {
    frameBufferResized = false;
}

void viewPort::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto viewWindow = reinterpret_cast<viewPort*>(glfwGetWindowUserPointer(window));

    viewWindow->frameBufferResized = true;
    viewWindow->WIDTH = width;
    viewWindow->HEIGHT = height;

}


void viewPort::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}