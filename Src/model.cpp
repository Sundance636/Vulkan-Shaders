#include "model.h"
#include "Utils.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

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
    vkDestroyBuffer(modelDevice.device(), vertexBuffer, nullptr);
    vkFreeMemory(modelDevice.device(), vertexBufferMemory, nullptr);

    if(hasIndexBuffer) {
        vkDestroyBuffer(modelDevice.device(), indexBuffer, nullptr);
        vkFreeMemory(modelDevice.device(), indexBufferMemory, nullptr);
    }
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;//number of bytes needed to store the vertex buffer

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    //stagin buffer
    modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,stagingBufferMemory);

    void* data;

    //create a reigon of host memory mapped to device memory
    vkMapMemory(modelDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    //coherent bit makes propagation automatic
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(modelDevice.device(), stagingBufferMemory);

    modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,vertexBufferMemory);

    //copy from staging buffer to device

    modelDevice.copyBuffer(stagingBuffer,vertexBuffer,bufferSize);

    vkDestroyBuffer(modelDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(modelDevice.device(), stagingBufferMemory, nullptr);

}

void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = (indexCount > 0);

    if(!hasIndexBuffer) {
        return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;//number of bytes needed to store the vertex buffer

    //staging buffers are iddeal for static data
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    //stagin buffer
    modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,stagingBufferMemory);

    void* data;

    //create a reigon of host memory mapped to device memory
    vkMapMemory(modelDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    //coherent bit makes propagation automatic
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(modelDevice.device(), stagingBufferMemory);

    modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBuffer,indexBufferMemory);

    //copy from staging buffer to device

    modelDevice.copyBuffer(stagingBuffer,indexBuffer,bufferSize);

    vkDestroyBuffer(modelDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(modelDevice.device(), stagingBufferMemory, nullptr);

}

void Model::draw(VkCommandBuffer commandBuffer) {
    if(hasIndexBuffer) {
        vkCmdDrawIndexed(commandBuffer,indexCount,1,0,0,0);
    } else {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0,1, buffers, offsets);

    if(hasIndexBuffer) {
        //index type should scale with the number of vertices
        vkCmdBindIndexBuffer(commandBuffer,indexBuffer,0,VK_INDEX_TYPE_UINT32);
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
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{2};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex,position);

    attributeDescriptions[1].binding = 0;//zero since interweaving
    attributeDescriptions[1].location = 1;//location for shader
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

std::unique_ptr<Model> Model::createModelFromFile(coreDevice& device, const std::string& filePath) {
    Builder builder{};
    builder.loadModel(filePath);

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

        uint32_t colorIndex = 3 * index.vertex_index + 2;
        if (colorIndex < attrib.colors.size()) {
          vertex.color = {
              attrib.colors[colorIndex - 2],
              attrib.colors[colorIndex - 1],
              attrib.colors[colorIndex - 0],
          };
        } else {
          vertex.color = {0.5f, 0.5f, 0.5f};  // set default color
        }
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