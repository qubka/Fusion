#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class VignetteFilter : public PostFilter {
    public:
        explicit VignetteFilter(const Pipeline::Stage& pipelineStage, float innerRadius = 0.15f, float outerRadius = 1.35f, float opacity = 0.85f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getInnerRadius() const { return innerRadius; }
        void setInnerRadius(float radius) { innerRadius = radius; }

        float getOuterRadius() const { return outerRadius; }
        void setOuterRadius(float radius) { outerRadius = radius; }

        float getOpacity() const { return opacity; }
        void setOpacity(float opacity) { this->opacity = opacity; }

    private:
        PushHandler pushScene;

        float innerRadius;
        float outerRadius;
        float opacity;
    };
}
