#ifndef __Application_h__
#define __Application_h__

#include "viewPort.h"
#include "pipeline.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class Application {

    private:


        //viewPort Specifications
        uint32_t winWIDTH = 640;
        uint32_t winHEIGHT = 480;
        viewPort ApplicationWindow = viewPort(winWIDTH,winHEIGHT,"Vulkan Renderer");
        coreDevice appDevice = coreDevice(ApplicationWindow);
        pipeline Pipeline = pipeline(appDevice,"vertexShaders","fragShaders",pipeline::defaultPipelineConfigInfo(winWIDTH,winHEIGHT));

    public:
        Application();

        void run();

};

#endif