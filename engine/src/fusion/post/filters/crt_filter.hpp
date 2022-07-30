#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class CrtFilter : public PostFilter {
    public:
        explicit CrtFilter(Pipeline::Stage pipelineStage, const glm::vec4& screenColor = {0.5f, 1.0f, 0.5f, 1.0f}, float curveAmountX = 0.1f,
            float curveAmountY = 0.1f, float scanLineSize = 1000.0f, float scanIntensity = 0.1f);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        const glm::vec4& getScreenColor() const { return screenColor; }
        void setScreenColor(const glm::vec4& value) { screenColor = value; }

        float getCurveAmountX() const { return curveAmountX; }
        void setCurveAmountX(float value) { curveAmountX = value; }

        float getCurveAmountY() const { return curveAmountY; }
        void setCurveAmountY(float value) { curveAmountY = value; }

        float hetScanLineSize() const { return scanLineSize; }
        void setScanLineSize(float value) { scanLineSize = value; }

        float hetScanIntensity() const { return scanIntensity; }
        void setScanIntensity(float value) { scanIntensity = value; }

    private:
        PushHandler pushScene;

        glm::vec4 screenColor;
        float curveAmountX, curveAmountY;
        float scanLineSize;
        float scanIntensity;
    };
}
