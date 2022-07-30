#include "negative_filter.hpp"

using namespace fe;

NegativeFilter::NegativeFilter(Pipeline::Stage pipelineStage)
        : PostFilter{pipelineStage, {"engine/assets/shaders/post/default.vert", "engine/assets/shaders/post/negative.frag"}} {
}

void NegativeFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates descriptors.
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
