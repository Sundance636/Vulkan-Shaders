#ifndef __Application_h__
#define __Application_h__

#include "viewPort.h"
#include "model.h"
#include "entity.h"
#include "Renderer.h"
#include "RenderSystem.h"

//forces radians across all platforms
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //maybe change later
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <memory>
#include <iostream>

class Application {

    private:

        //viewPort Specifications
        uint32_t winWIDTH = 640;
        uint32_t winHEIGHT = 480;
        viewPort ApplicationWindow = viewPort(winWIDTH,winHEIGHT,"Vulkan Renderer");

        coreDevice appDevice = coreDevice(ApplicationWindow);
        Renderer appRenderer{ApplicationWindow, appDevice};

        std::vector<Entity> entities;

        void loadEntities();

    public:
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application &operator=(const Application&) = delete;

        void run();

};

#endif