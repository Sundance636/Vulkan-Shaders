#ifndef __camera_h__
#define __camera_h__

//forces radians across all platforms
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //maybe change later
#include <glm/glm.hpp>
#include <cassert>
#include <limits>

class Camera {

    private:

        glm::mat4 projectionMatrix{1.0f};

        


    public:

        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float FOVy, float aspectRatio, float near, float far);
        
        const glm::mat4& getProjection() const;



};

#endif