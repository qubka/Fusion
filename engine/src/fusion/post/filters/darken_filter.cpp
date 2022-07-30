#include "darken_filter.hpp"

using namespace fe;

DarkenFilter::DarkenFilter(Pipeline::Stage pipelineStage, float factor)
        : PostFilter{pipelineStage, {"engine/assets/shaders/post/default.vert", "engine/assets/shaders/post/darken.frag"}}
        , factor{factor} {
}

void DarkenFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("factor", factor);

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
