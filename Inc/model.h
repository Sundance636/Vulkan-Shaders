#ifndef __model_h__
#define __model_h__

#include "coreDevice.h"

//forces radians across all platforms
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //maybe change later
#include <glm/glm.hpp>
#include <cassert>
#include <cstring>

struct Transform2dComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.0f, 1.0f};
    float rotation;

    glm::mat2 mat2() { 
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);

        glm::mat2 rotMatrix{{c, s}, {-s, c}};

        glm::mat2 scaleMat{{scale.x, 0.0f},
                            {0.0f, scale.y}};

        return rotMatrix * scaleMat;
        }
};



//will contain the vertices of some model
class Model {
    public:
        struct Vertex {
            glm::vec2 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        Model(coreDevice &device, const std::vector<Vertex> &vertices);
        ~Model();
        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        void bind(VkCommandBuffer VkCommandBuffer);
        void draw(VkCommandBuffer VkCommandBuffer);

    private:
        coreDevice &modelDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
        

        void createVertexBuffers(const std::vector<Vertex> &vertices);

};


#endif