#include "pixel_filter.hpp"

using namespace fe;

PixelFilter::PixelFilter(const Pipeline::Stage& pipelineStage, float pixelSize)
    : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/pixel.frag"}}, pixelSize{pixelSize} {
}

void PixelFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("pixelSize", pixelSize);

    // Updates descriptors.
    descriptorSet.push("PushScene", pushScene);
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushScene.BindPush(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
