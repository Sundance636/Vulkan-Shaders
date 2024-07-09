#include "model.h"

Model::Model(coreDevice &device, const std::vector<Vertex> &vertices) : modelDevice{device} {
    createVertexBuffers(vertices);
}

Model::~Model() {
    vkDestroyBuffer(modelDevice.device(), vertexBuffer, nullptr);
    vkFreeMemory(modelDevice.device(), vertexBufferMemory, nullptr);
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;//number of bytes needed to store the vertex buffer

    modelDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer,vertexBufferMemory);

    void* data;

    //create a reigon of host memory mapped to device memory
    vkMapMemory(modelDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
    //coherent bit makes propagation automatic
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(modelDevice.device(), vertexBufferMemory);

}


void Model::draw(VkCommandBuffer commandBuffer) {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0,1, buffers, offsets);
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
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;//rgb <-> vec3
    attributeDescriptions[0].offset = offsetof(Vertex,position);

    attributeDescriptions[1].binding = 0;//zero since interweaving
    attributeDescriptions[1].location = 1;//location for shader
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}
