#pragma once

#include "image.hpp"

namespace fe {
    /**
     * @brief Resource that represents a depth stencil image.
     */
    class ImageDepth : public Image {
    public:
        explicit ImageDepth(const glm::uvec2& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    };
}
