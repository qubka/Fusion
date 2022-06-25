#pragma once

#include "fusion/graphics/pipelines/shader.hpp"
#include "fusion/utils/math.hpp"

namespace fe {
    struct Vertex2d {
        glm::vec2 position;
        glm::vec2 uv;

        static Shader::VertexInput GetVertexInput(uint32_t baseBinding = 0) {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
                    {baseBinding, sizeof(Vertex2d), VK_VERTEX_INPUT_RATE_VERTEX}
            };
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
                    {0, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2d, position)},
                    {1, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2d, uv)}
            };
            return {bindingDescriptions, attributeDescriptions};
        }

        bool operator==(const Vertex2d& rhs) const {
            return position == rhs.position && uv == rhs.uv;
        }

        bool operator!=(const Vertex2d& rhs) const {
            return !operator==(rhs);
        }
    };
}

namespace std {
    template<>
    struct hash<fe::Vertex2d> {
        size_t operator()(const fe::Vertex2d& vertex) const noexcept {
            size_t seed = 0;
            fe::Math::HashCombine(seed, vertex.position);
            fe::Math::HashCombine(seed, vertex.uv);
            return seed;
        }
    };
}