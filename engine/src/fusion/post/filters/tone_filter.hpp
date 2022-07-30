#pragma once

#include "fusion/post/post_filter.hpp"

namespace fe {
    class ToneFilter : public PostFilter {
    public:
        explicit ToneFilter(Pipeline::Stage pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;
    };
}
