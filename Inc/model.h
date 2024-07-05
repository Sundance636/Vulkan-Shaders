#ifndef __model_h__
#define __model_h__

#include "coreDevice.h"
#include "Buffer.h"

//forces radians across all platforms
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //maybe change later
#include <glm/glm.hpp>
#include <cassert>
#include <cstring>
#include <memory>
#include <iostream>
#include <unordered_map>



//will contain the vertices of some model
class Model {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 UV{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        
            bool operator==(const Vertex &other) const {
                return position == other.position && color == other.color && normal == other.normal && UV == other.UV;
            }
        };

        //for index buffer
        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indicies{};

            void loadModel(const std:: string & filePath);

        };

        Model(coreDevice &device, const Model::Builder &builder);
        ~Model();
        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        void bind(VkCommandBuffer VkCommandBuffer);
        void draw(VkCommandBuffer VkCommandBuffer);
        static std::unique_ptr<Model> createModelFromFile(coreDevice& device, const std::string& filePath);

    private:
        coreDevice &modelDevice;

        std::unique_ptr<Buffer> vertexBuffer;
        uint32_t vertexCount;

        std::unique_ptr<Buffer> indexBuffer;
        uint32_t indexCount;
        bool hasIndexBuffer = false;
        

        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

};


#endif