#pragma once

#include "fusion/utils/math.hpp"

namespace fe {
    struct VertexImgui {
        glm::vec2 position;
        glm::vec2 uv;
        glm::vec4 color;

        static Shader::VertexInput GetVertexInput(uint32_t baseBinding = 0) {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
                    {baseBinding, sizeof(VertexImgui), VK_VERTEX_INPUT_RATE_VERTEX}
            };
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
                    {0, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexImgui, position)},
                    {1, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexImgui, uv)},
                    {2, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexImgui, color)}
            };
            return {bindingDescriptions, attributeDescriptions};
        }

        bool operator==(const VertexImgui& rhs) const {
            return position == rhs.position && uv == rhs.uv && color == rhs.color;
        }

        bool operator!=(const VertexImgui& rhs) const {
            return !operator==(rhs);
        }
    };
}

namespace std {
    template<>
    struct hash<fe::VertexImgui> {
        size_t operator()(const fe::VertexImgui& vertex) const noexcept {
            size_t seed = 0;
            fe::Math::HashCombine(seed, vertex.position);
            fe::Math::HashCombine(seed, vertex.uv);
            fe::Math::HashCombine(seed, vertex.color);
            return seed;
        }
    };
}
