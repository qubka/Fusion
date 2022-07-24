#include "grain_filter.hpp"

using namespace fe;

GrainFilter::GrainFilter(const Pipeline::Stage& pipelineStage, float strength)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/grain.frag"}}
        , strength{strength} {
}

void GrainFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("strength", strength);

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
