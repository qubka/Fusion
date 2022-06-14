#pragma once

#include "buffer.hpp"
#include "context.hpp"

namespace vkx {
    struct Mesh {
        Buffer vertices;
        Buffer indices;
        uint32_t indexCount{ 0 };
        uint32_t vertexCount{ 0 };

        struct Vertex {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec3 normal;
            glm::vec2 uv;

            Vertex(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& normal, const glm::vec2& uv)
                : pos{pos}, color{color}, normal{normal}, uv{uv} {}
        };

        /** @brief Generate mesh from list of vertices and indices */
        void loadFromBuffer(const Context& context, const std::vector<Vertex>& vertexBuffer, const std::vector<uint32_t>& indexBuffer = {}) {
            vertexCount = static_cast<uint32_t>(vertexBuffer.size());
            assert(vertexCount >= 3 && "Vertex count must be at least 3");

            // Vertex buffer
            vertices = context.stageToDeviceBuffer<Vertex>(vk::BufferUsageFlagBits::eVertexBuffer, vertexBuffer);

            indexCount = static_cast<uint32_t>(indexBuffer.size());

            // Index buffer
            indices = indexCount > 0 ? context.stageToDeviceBuffer<uint32_t>(vk::BufferUsageFlagBits::eIndexBuffer, indexBuffer) : Buffer{};
        }

        /** @brief Release all Vulkan resources of this mesh */
        void destroy() {
            vertices.destroy();
            indices.destroy();
        }
    };
}
