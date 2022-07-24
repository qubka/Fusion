#pragma once

#include "fusion/post/post_filter.hpp"

namespace fe {
    class BlitFilter : public PostFilter {
    public:
        explicit BlitFilter(const Pipeline::Stage& pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;
    };
}
