#pragma once

#include "fusion/post/post_filter.hpp"

namespace fe {
    class SepiaFilter : public PostFilter {
    public:
        explicit SepiaFilter(Pipeline::Stage pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;
    };
}
