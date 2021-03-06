#include "blur_pipeline.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/textures/texture2d.hpp"

namespace fe {
    BlurPipeline::BlurPipeline(const Pipeline::Stage& pipelineStage, float blur, const BlurFilter::Type &blurType, bool toScreen, float inputScale, float outputScale)
            : PostPipeline{pipelineStage}
            , filterBlurVertical{pipelineStage, {0.0f, blur}, blurType}
            , filterBlurHorizontal{pipelineStage, {blur, 0.0f}, blurType}
            , toScreen{toScreen}
            , inputScale{inputScale}
            , outputScale{outputScale}
            , blur{blur} {
    }

    void BlurPipeline::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
        if (!toScreen) {
            if (auto size = DeviceManager::Get()->getWindow(0)->getSize(); size != lastSize) {
                auto newSize = outputScale * glm::vec2{size};
                output = std::make_unique<Texture2d>(newSize, VK_FORMAT_R8G8B8A8_UNORM);

                filterBlurVertical.setAttachment("writeColour", output.get());
                filterBlurHorizontal.setAttachment("writeColour", output.get());

                lastSize = size;
            }
        }

        filterBlurVertical.SetDirection({0.0f, blur});
        filterBlurHorizontal.SetDirection({blur, 0.0f});

        filterBlurVertical.Render(commandBuffer);
        filterBlurHorizontal.Render(commandBuffer);
    }
}
