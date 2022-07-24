#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class GrainFilter : public PostFilter {
    public:
        explicit GrainFilter(const Pipeline::Stage& pipelineStage, float strength = 2.3f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getStrength() const { return strength; }
        void setStrength(float strength) { this->strength = strength; }

    private:
        PushHandler pushScene;

        float strength;
    };
}
