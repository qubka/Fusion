#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/post/pipelines/blur_pipeline.hpp"

namespace fe {
    class DofFilter : public PostFilter {
    public:
        explicit DofFilter(const Pipeline::Stage& pipelineStage, BlurPipeline* pipelineBlur, float focusPoint = 8.24f, float nearField = 0.3f,
            float nearTransition = 0.125f, float farField = 8.0f, float farTransition = 3.0f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        const BlurPipeline* getPipelineBlur() const { return pipelineBlur; }
        void setPipelineBlur(BlurPipeline* pipelineBlur) { this->pipelineBlur = pipelineBlur; }

        float getFocusPoint() const { return focusPoint; }
        void setFocusPoint(float focusPoint) { this->focusPoint = focusPoint; }

        float getNearField() const { return nearField; }
        void setNearField(float nearField) { this->nearField = nearField; }

        float getNearTransition() const { return nearTransition; }
        void setNearTransition(float nearTransition) { this->nearTransition = nearTransition; }

        float getFarField() const { return farField; }
        void setFarField(float farField) { this->farField = farField; }

        float getFarTransition() const { return farTransition; }
        void setFarTransition(float farTransition) { this->farTransition = farTransition; }

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
