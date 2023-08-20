#pragma once

#include "fusion/graphics/textures/texture.h"

namespace fe {
    /**
     * @brief Resource that represents a depth stencil image.
     */
    class FUSION_API TextureDepth final : public Texture {
    public:
        explicit TextureDepth(const glm::uvec2& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

        type_index getType() const override { return type_id<TextureDepth>; }

        void load() override {};
        void unload() override {};
    };
}
