#include "emboss_filter.hpp"

using namespace fe;

EmbossFilter::EmbossFilter(Pipeline::Stage pipelineStage)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/emboss.frag"}} {
}

void EmbossFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates descriptors.
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
