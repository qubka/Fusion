#pragma once

#include "fusion/graphics/pipelines/shader.hpp"

namespace fe {
    class Grid {
    public:
        static Shader::VertexInput GetVertexInput(uint32_t baseBinding = 0) {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
                    {baseBinding, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX}
            };
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
                    {0, baseBinding, VK_FORMAT_R32G32_SFLOAT, 0},
            };
            return {bindingDescriptions, attributeDescriptions};
        }
    };
}
