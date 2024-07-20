#include "model.h"
#include "Utils.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace std {
template <>
struct hash<Model::Vertex> {
  size_t operator()(Model::Vertex const &vertex) const {
    size_t seed = 0;
    lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.UV);
    return seed;
  }
};
}

Model::Model(coreDevice &device, const Model::Builder &builder) : modelDevice{device} {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indicies);
}

Model::~Model() {

}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;//number of bytes needed to store the vertex buffer
    uint32_t vertexSize = sizeof(vertices[0]);

    Buffer stagingBuffer = {
        modelDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)vertices.data());

    vertexBuffer = std::make_unique<Buffer>(
        modelDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    modelDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(),bufferSize);


}

void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = (indexCount > 0);

    if(!hasIndexBuffer) {
        return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;//number of bytes needed to store the vertex buffer
    uint32_t indexSize = sizeof(indices[0]);

    Buffer stagingBuffer = {
        modelDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)indices.data());

    indexBuffer = std::make_unique<Buffer>(
        modelDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    modelDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(),bufferSize);

}

void Model::draw(VkCommandBuffer commandBuffer) {
    if(hasIndexBuffer) {
        vkCmdDrawIndexed(commandBuffer,indexCount,1,0,0,0);
    } else {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0,1, buffers, offsets);

    if(hasIndexBuffer) {
        //index type should scale with the number of vertices
        vkCmdBindIndexBuffer(commandBuffer,indexBuffer->getBuffer(),0,VK_INDEX_TYPE_UINT32);
    }
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions{1};

    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{4};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex,position);

    attributeDescriptions[1].binding = 0;//zero since interweaving
    attributeDescriptions[1].location = 1;//location for shader
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex,normal);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex,UV);

    return attributeDescriptions;
}

std::unique_ptr<Model> Model::createModelFromFile(coreDevice& device, const std::string& filePath) {
    Builder builder{};
    builder.loadModel( ENGINE_DIR + filePath);

    std::cout << "Vertex Count: " << builder.vertices.size() << '\n';

    return std::make_unique<Model>(device, builder);
}


void Model::Builder::loadModel(const std::string &filepath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;


  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
    throw std::runtime_error(warn + err);
  }

  vertices.clear();
  indicies.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      if (index.vertex_index >= 0) {
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };

        vertex.color = {
            attrib.colors[3 * index.vertex_index + 0],
            attrib.colors[3 * index.vertex_index + 1],
            attrib.colors[3 * index.vertex_index + 2],
        };

        //vertex.color = {0.5f,0.5f,0.5f};

        
      }

      if (index.normal_index >= 0) {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
        };
      }

      if (index.texcoord_index >= 0) {
        vertex.UV = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            attrib.texcoords[2 * index.texcoord_index + 1],
        };
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indicies.push_back(uniqueVertices[vertex]);
    }
  }
}