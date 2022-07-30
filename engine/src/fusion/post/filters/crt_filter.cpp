#include "crt_filter.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

CrtFilter::CrtFilter(Pipeline::Stage pipelineStage, const glm::vec4& screenColor, float curveAmountX, float curveAmountY, float scanLineSize, float scanIntensity)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/crt.frag"}}
        , screenColor{screenColor}
        , curveAmountX{curveAmountX}
        , curveAmountY{curveAmountY}
        , scanLineSize{scanLineSize}
        , scanIntensity{scanIntensity} {
}

void CrtFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("screenColor", screenColor);
    pushScene.push("curveAmountX", curveAmountX * pipeline.getRenderArea().getAspectRatio());
    pushScene.push("curveAmountY", curveAmountY);
    pushScene.push("scanLineSize", scanLineSize);
    pushScene.push("scanIntensity", scanIntensity);
    pushScene.push("moveTime", DateTime::Now() / 100.0f);

    // Updates descriptors.
    descriptorSet.push("PushScene", pushScene);
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushScene.bindPush(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
