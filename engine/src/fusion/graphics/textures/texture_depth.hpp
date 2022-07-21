#pragma once

#include "texture.hpp"

namespace fe {
    /**
     * @brief Resource that represents a depth stencil image.
     */
    class TextureDepth : public Texture {
    public:
        explicit TextureDepth(const glm::uvec2& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    };
}
