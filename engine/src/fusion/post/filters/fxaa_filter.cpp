#include "fxaa_filter.hpp"

using namespace fe;

FxaaFilter::FxaaFilter(const Pipeline::Stage& pipelineStage, float spanMax)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/fxaa.frag"}}
        , spanMax{spanMax} {
}

void FxaaFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("spanMax", spanMax);

    // Updates descriptors.
    descriptorSet.push("PushScene", pushScene);
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Binds the pipeline.
    pipeline.bindPipeline(commandBuffer);

    // Draws the object.
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushScene.bindPush(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
