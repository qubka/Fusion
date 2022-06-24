#pragma once

#include <volk.h>

namespace vku {
    template<typename T> static constexpr T numMipmapLevels(T width, T height) {
        T levels = 1;
        while((width|height) >> levels) {
            ++levels;
        }
        return levels;
    }

    inline VkViewport viewport(float x, float y, float width, float height, float minDepth = 0, float maxDepth = 1) {
        return { x, y, width, height, minDepth, maxDepth };
    }

    inline VkViewport viewport(const glm::uvec2& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, 0, static_cast<float>(size.x), static_cast<float>(size.y), minDepth, maxDepth);
    }

    inline VkViewport viewport(const VkExtent2D& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, 0, static_cast<float>(size.width), static_cast<float>(size.height), minDepth, maxDepth);
    }

    inline VkViewport flippedViewport(const glm::uvec2& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, static_cast<float>(size.y), static_cast<float>(size.x), -static_cast<float>(size.y), minDepth, maxDepth);
    }

    inline VkViewport flippedViewport(const VkExtent2D& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, static_cast<float>(size.height), static_cast<float>(size.width), -static_cast<float>(size.height), minDepth, maxDepth);
    }

    inline VkRect2D rect2D(uint32_t width, uint32_t height, int32_t offsetX = 0, int32_t offsetY = 0) {
        return {{offsetX,offsetY},{width,height}};
    }

    inline VkRect2D rect2D(const glm::uvec2& size, const glm::ivec2& offset = {}) {
        return rect2D(size.x, size.y, offset.x, offset.y);
    }

    inline VkRect2D rect2D(const VkExtent2D& size, const VkOffset2D& offset = {}) {
        return rect2D(size.width, size.height, offset.x, offset.y);
    }

    inline VkClearColorValue clearColor(const glm::vec4& v = glm::vec4{0.0f}) {
        VkClearColorValue result;
        memcpy(&result.float32, &v, sizeof(result.float32));
        return result;
    }
}