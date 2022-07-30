#pragma once

#include "fusion/post/post_filter.hpp"

namespace fe {
    class DefaultFilter : public PostFilter {
    public:
        explicit DefaultFilter(Pipeline::Stage pipelineStage, bool lastFilter = false);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

    private:
        bool lastFilter;
    };
}
