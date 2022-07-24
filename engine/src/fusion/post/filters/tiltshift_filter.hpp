#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class TiltshiftFilter : public PostFilter {
    public:
        explicit TiltshiftFilter(const Pipeline::Stage& pipelineStage, float blurAmount = 1.0f, float centre = 1.1f, float stepSize = 0.004f, float steps = 3.0f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getBlurAmount() const { return blurAmount; }
        void setBlurAmount(float blurAmount) { this->blurAmount = blurAmount; }

        float getCentre() const { return centre; }
        void setCentre(float centre) { this->centre = centre; }

        float getStepSize() const { return stepSize; }
        void setStepSize(float stepSize) { this->stepSize = stepSize; }

        float getSteps() const { return steps; }
        void setSteps(float steps) { this->steps = steps; }

    private:
        PushHandler pushScene;

        float blurAmount;
        float centre;
        float stepSize;
        float steps;
    };
}
