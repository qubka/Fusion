#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class VignetteFilter : public PostFilter {
    public:
        explicit VignetteFilter(const Pipeline::Stage& pipelineStage, float innerRadius = 0.15f, float outerRadius = 1.35f, float opacity = 0.85f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getInnerRadius() const { return innerRadius; }
        void setInnerRadius(float value) { innerRadius = value; }

        float getOuterRadius() const { return outerRadius; }
        void setOuterRadius(float value) { outerRadius = value; }

        float getOpacity() const { return opacity; }
        void setOpacity(float value) { opacity = value; }

    private:
        PushHandler pushScene;

        float innerRadius;
        float outerRadius;
        float opacity;
    };
}
