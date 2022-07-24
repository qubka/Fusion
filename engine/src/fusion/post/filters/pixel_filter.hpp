#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class PixelFilter : public PostFilter {
    public:
        explicit PixelFilter(const Pipeline::Stage& pipelineStage, float pixelSize = 2.0f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float GetPixelSize() const { return pixelSize; }
        void SetPixelSize(float pixelSize) { this->pixelSize = pixelSize; }

    private:
        PushHandler pushScene;

        float pixelSize;
    };
}
