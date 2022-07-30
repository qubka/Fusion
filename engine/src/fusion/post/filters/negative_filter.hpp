#pragma once

#include "fusion/post/post_filter.hpp"

namespace fe {
    class NegativeFilter : public PostFilter {
    public:
        explicit NegativeFilter(Pipeline::Stage pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;
    };
}
