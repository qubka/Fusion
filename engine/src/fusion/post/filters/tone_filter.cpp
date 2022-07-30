#include "tone_filter.hpp"

using namespace fe;

ToneFilter::ToneFilter(Pipeline::Stage pipelineStage)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/tone.frag"}} {
}

void ToneFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates descriptors.
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
