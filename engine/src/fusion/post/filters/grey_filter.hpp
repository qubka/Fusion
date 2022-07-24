#pragma once

#include "fusion/post/post_filter.hpp"

namespace fe {
    class GreyFilter : public PostFilter {
    public:
        explicit GreyFilter(const Pipeline::Stage& pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;
    };
}
