#include "mesh.hpp"

using namespace vkx;

void Mesh::loadFromBuffer(const Context& context, const std::vector<Vertex>& vertexBuffer, const std::vector<uint32_t>& indexBuffer) {
    vertexCount = static_cast<uint32_t>(vertexBuffer.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");

    // Vertex buffer
    vertices = context.stageToDeviceBuffer(vk::BufferUsageFlagBits::eVertexBuffer, vertexBuffer);

    indexCount = static_cast<uint32_t>(indexBuffer.size());

    // Index buffer
    indices = indexCount > 0 ? context.stageToDeviceBuffer(vk::BufferUsageFlagBits::eIndexBuffer, indexBuffer) : Buffer{};
}

void Mesh::destroy() {
    vertices.destroy();
    indices.destroy();
}

void Mesh::draw(const vk::CommandBuffer& commandBuffer) const {
    if (indices) {
        commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
    } else {
        commandBuffer.draw(vertexCount, 1, 0, 0);
    }
}

void Mesh::bind(const vk::CommandBuffer& commandBuffer) const {
    commandBuffer.bindVertexBuffers(0, vertices.buffer, { 0 });

    if (indices) {
        commandBuffer.bindIndexBuffer(indices.buffer, 0, vk::IndexType::eUint32);
    }
}