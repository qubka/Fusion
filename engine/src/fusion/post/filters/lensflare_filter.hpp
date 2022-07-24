#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class LensflareFilter : public PostFilter {
    public:
        explicit LensflareFilter(const Pipeline::Stage& pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        const glm::vec3& getSunPosition() const { return sunPosition; }
        void setSunPosition(const glm::vec3& sunPosition, const glm::vec2& screenSize);

        float getSunHeight() const { return sunHeight; }
        void setSunHeight(float height) { sunHeight = height; }

    private:
        PushHandler pushScene;

        glm::vec3 sunPosition;
        float sunHeight{ 0.0f };
    };
}
