#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class DarkenFilter : public PostFilter {
    public:
        explicit DarkenFilter(const Pipeline::Stage& pipelineStage, float factor = 0.5f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getFactor() const { return factor; }
        void setFactor(float factor) { this->factor = factor; }

    private:
        PushHandler pushScene;

        float factor;
    };
}
