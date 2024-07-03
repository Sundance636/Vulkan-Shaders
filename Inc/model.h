#ifndef __model_h__
#define __model_h__

#include "coreDevice.h"

//forces radians across all platforms
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //maybe change later
#include <glm/glm.hpp>
#include <cassert>
#include <cstring>



//will contain the vertices of some model
class Model {
    public:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        //for index buffer
        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indicies{};
        };


        Model(coreDevice &device, const Model::Builder &builder);
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

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
        bool hasIndexBuffer = false;
        

        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

};


#endif