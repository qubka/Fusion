#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class FxaaFilter : public PostFilter {
    public:
        explicit FxaaFilter(const Pipeline::Stage& pipelineStage, float spanMax = 8.0f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getSpanMax() const { return spanMax; }
        void setSpanMax(float span) { spanMax = span; }

    private:
        PushHandler pushScene;

        float spanMax;
    };
}
