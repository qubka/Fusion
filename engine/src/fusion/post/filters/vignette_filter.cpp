#include "vignette_filter.hpp"

using namespace fe;

VignetteFilter::VignetteFilter(const Pipeline::Stage& pipelineStage, float innerRadius, float outerRadius, float opacity)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/vignette.frag"}}
        , innerRadius{innerRadius}
        , outerRadius{outerRadius}
        , opacity{opacity} {
}

void VignetteFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("innerRadius", innerRadius);
    pushScene.push("outerRadius", outerRadius);
    pushScene.push("opacity", opacity);

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
