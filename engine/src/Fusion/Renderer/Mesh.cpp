#include "Mesh.hpp"

//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>

using namespace Fusion;

Mesh::Mesh(Vulkan& vulkan, const Builder& builder) : vulkan{vulkan} {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

Mesh::~Mesh() {
}

void Mesh::createVertexBuffers(const std::vector<Vertex>& vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    FE_ASSERT(vertexCount >= 3 && "Vertex count must be at least 3");
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);

    AllocatedBuffer stagingBuffer{
            vulkan,
            vertexSize,
            vertexCount,
            vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer = std::make_unique<AllocatedBuffer>(
        vulkan,
        vertexSize,
        vertexCount,
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    vulkan.copyBuffer(stagingBuffer.get(), vertexBuffer->get(), bufferSize);
}

void Mesh::createIndexBuffers(const std::vector<uint32_t>& indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = indexCount > 0;

    if (!hasIndexBuffer) {
        return;
    }

    vk::DeviceSize bufferSize = sizeof(indices[0]) * indexCount;
    uint32_t indexSize = sizeof(indices[0]);

    AllocatedBuffer stagingBuffer {
            vulkan,
            indexSize,
            indexCount,
            vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    indexBuffer = std::make_unique<AllocatedBuffer>(
            vulkan,
            indexSize,
            indexCount,
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal);

    vulkan.copyBuffer(stagingBuffer.get(), indexBuffer->get(), bufferSize);
}

void Mesh::draw(const vk::CommandBuffer& commandBuffer) const {
    if (hasIndexBuffer) {
        commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
    } else {
        commandBuffer.draw(vertexCount, 1, 0, 0);
    }
}

void Mesh::bind(const vk::CommandBuffer& commandBuffer) const {
    vk::Buffer buffers[] = {vertexBuffer->get()};
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, 1, buffers, offsets);

    if (hasIndexBuffer) {
        commandBuffer.bindIndexBuffer(indexBuffer->get(), 0, vk::IndexType::eUint32);
    }
}

std::vector<vk::VertexInputBindingDescription> Mesh::Vertex::getBindingDescriptions() {
    return {
        {0, sizeof(Vertex), vk::VertexInputRate::eVertex}
    };
}

std::vector<vk::VertexInputAttributeDescription> Mesh::Vertex::getAttributeDescriptions() {
    return {
        {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
        {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
        {2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, ui)},
    };
}

namespace std {
    template <>
    struct hash<Mesh::Vertex> {
        size_t operator()(Mesh::Vertex const &vertex) const {
            size_t seed = 0;
            hashCombine(seed, vertex.position, vertex.normal, vertex.ui);
            return seed;
        }
    };
}  // namespace std

void Mesh::Builder::loadModel(const std::string &filepath) {
    /*tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        /FE_ASSERT(warn + err);
    }

    vertices.clear();
    indices.clear();

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
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.ui = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }*/
}