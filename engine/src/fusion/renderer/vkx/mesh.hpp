#pragma once

#include "buffer.hpp"
#include "context.hpp"

namespace vkx {
    struct Mesh {
        Buffer vertices;
        Buffer indices;
        uint32_t indexCount { 0 };
        uint32_t vertexCount { 0 };

        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 ui{};
        };

        /** @brief Generate mesh from list of vertices and indices */
        void loadFromBuffer(const Context& context, const std::vector<Vertex>& vertexBuffer, const std::vector<uint32_t>& indexBuffer = {});

        /** @brief Release all Vulkan resources of this mesh */
        void destroy();

        void bind(const vk::CommandBuffer& commandBuffer) const;
        void draw(const vk::CommandBuffer& commandBuffer) const;
    };
}
