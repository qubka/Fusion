#include "default_filter.hpp"

using namespace fe;

DefaultFilter::DefaultFilter(Pipeline::Stage pipelineStage, bool lastFilter)
        : PostFilter{pipelineStage, {"engine/assets/shaders/post/default.vert", "engine/assets/shaders/post/default.frag"}}
        , lastFilter{lastFilter} {
}

void DefaultFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates descriptors.
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // Resets switching for next pass.
    if (lastFilter) {
        GlobalSwitching = 0;
    }
}
