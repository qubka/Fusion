#pragma once

#include "buffer.hpp"
#include "context.hpp"

namespace vkx {
    class Mesh {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 ui{};
        };

        Mesh(const Context& context, const std::vector<Vertex>& vertexBuffer, const std::vector<uint32_t>& indexBuffer = {});

        /** @brief Release all Vulkan resources of this mesh */
        void destroy();

        void bind(const vk::CommandBuffer& cmdBuffer) const;
        void draw(const vk::CommandBuffer& cmdBuffer) const;

    private:
        Buffer vertices;
        Buffer indices;
        uint32_t indexCount;
        uint32_t vertexCount;
    };
}
