#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class TiltshiftFilter : public PostFilter {
    public:
        explicit TiltshiftFilter(Pipeline::Stage pipelineStage, float blurAmount = 1.0f, float centre = 1.1f, float stepSize = 0.004f, float steps = 3.0f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getBlurAmount() const { return blurAmount; }
        void setBlurAmount(float value) { blurAmount = value; }

        float getCentre() const { return centre; }
        void setCentre(float value) { centre = value; }

        float getStepSize() const { return stepSize; }
        void setStepSize(float value) { stepSize = value; }

        float getSteps() const { return steps; }
        void setSteps(float value) { steps = value; }

    private:
        PushHandler pushScene;

        float blurAmount;
        float centre;
        float stepSize;
        float steps;
    };
}
