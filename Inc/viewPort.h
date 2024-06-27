#ifndef __viewPort_h__
#define __viewPort_h__

#include <stdexcept>
#include <string>

#define GLFW_INCLUDE_VULKAN //makes GFLW incluce vulkan along with it
#include <GLFW/glfw3.h>

class viewPort {

    private:
        GLFWwindow *window;

        //Window Specifications
        uint32_t WIDTH = 640;
        uint32_t HEIGHT = 480;
        std::string windowName;

        //private Initialization functions
        void initWindow();


        public:
            viewPort();
            viewPort( u_int32_t Width, uint32_t Height, std::string name);
            ~viewPort();
            bool shouldClose();

            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

            //overloads
            viewPort(const viewPort&) = delete;
            viewPort &operator=(const viewPort&) = delete;
            

};

#endif