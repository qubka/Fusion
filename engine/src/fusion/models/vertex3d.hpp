#pragma once

#include "fusion/graphics/pipelines/shader.hpp"
#include "fusion/utils/math.hpp"

namespace fe {
    struct Vertex3d {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;

        static Shader::VertexInput GetVertexInput(uint32_t baseBinding = 0) {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
                    {baseBinding, sizeof(Vertex3d), VK_VERTEX_INPUT_RATE_VERTEX}
            };
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
                    {0, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex3d, position)},
                    {1, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex3d, uv)},
                    {2, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex3d, normal)}
            };
            return {bindingDescriptions, attributeDescriptions};
        }

        bool operator==(const Vertex3d& rhs) const {
            return position == rhs.position && uv == rhs.uv && normal == rhs.normal;
        }

        bool operator!=(const Vertex3d& rhs) const {
            return !operator==(rhs);
        }
    };
}

namespace std {
    template<>
    struct hash<fe::Vertex3d> {
        size_t operator()(const fe::Vertex3d& vertex) const noexcept {
            size_t seed = 0;
            fe::Math::HashCombine(seed, vertex.position);
            fe::Math::HashCombine(seed, vertex.uv);
            fe::Math::HashCombine(seed, vertex.normal);
            return seed;
        }
    };
}
