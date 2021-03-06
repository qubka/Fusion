#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class WobbleFilter : public PostFilter {
    public:
        explicit WobbleFilter(const Pipeline::Stage& pipelineStage, float wobbleSpeed = 2.0f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getWobbleSpeed() const { return wobbleSpeed; }
        void setWobbleSpeed(float speed) { wobbleSpeed = speed; }

    private:
        PushHandler pushScene;

        float wobbleSpeed;
        float wobbleAmount;
    };
}
