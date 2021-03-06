#pragma once

#include "fusion/post/filters/blur_filter.hpp"
#include "fusion/post/post_pipeline.hpp"

namespace fe {
    class Texture2d;
    class BlurPipeline : public PostPipeline {
    public:
        explicit BlurPipeline(const Pipeline::Stage& pipelineStage, float blur = 2.0f, const BlurFilter::Type& blurType = BlurFilter::Type::_9, bool toScreen = false, float inputScale = 0.5f, float outputScale = 1.0f);

        void onUpdate() override {}
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        float getInputScale() const { return inputScale; }
        void setInputScale(float scale) { inputScale = scale; }
        float getOutputScale() const { return outputScale; }
        void setOutputScale(float scale) { outputScale = scale; }
        float getBlur() const { return blur; }
        void setBlur(float blurr) { blur = blurr; }

        const Texture2d* getOutput() const { return output.get(); }

    private:
        BlurFilter filterBlurVertical, filterBlurHorizontal;

        bool toScreen;
        float inputScale;
        float outputScale;
        float blur;

        std::unique_ptr<Texture2d> output;
        glm::uvec2 lastSize;
    };
}
