#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/post/pipelines/blur_pipeline.hpp"

namespace fe {
    class DofFilter : public PostFilter {
    public:
        explicit DofFilter(Pipeline::Stage pipelineStage, BlurPipeline* pipelineBlur, float focusPoint = 8.24f, float nearField = 0.3f,
            float nearTransition = 0.125f, float farField = 8.0f, float farTransition = 3.0f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        const BlurPipeline* getPipelineBlur() const { return pipelineBlur; }
        void setPipelineBlur(BlurPipeline* pipeline) { pipelineBlur = pipeline; }

        float getFocusPoint() const { return focusPoint; }
        void setFocusPoint(float value) { focusPoint = value; }

        float getNearField() const { return nearField; }
        void setNearField(float value) { nearField = value; }

        float getNearTransition() const { return nearTransition; }
        void setNearTransition(float value) { nearTransition = value; }

        float getFarField() const { return farField; }
        void setFarField(float value) { farField = value; }

        float getFarTransition() const { return farTransition; }
        void setFarTransition(float value) { farTransition = value; }

    private:
        PushHandler pushScene;

        BlurPipeline* pipelineBlur;

        float focusPoint;
        float nearField;
        float nearTransition;
        float farField;
        float farTransition;
    };
}
