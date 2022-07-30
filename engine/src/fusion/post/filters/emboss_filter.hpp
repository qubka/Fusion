#pragma once

#include "fusion/post/post_filter.hpp"

namespace fe {
    class EmbossFilter : public PostFilter {
    public:
        explicit EmbossFilter(Pipeline::Stage pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;
    };
}
