#include "blit_filter.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

BlitFilter::BlitFilter(Pipeline::Stage pipelineStage)
        : PostFilter{pipelineStage, {"engine/assets/shaders/post/default.vert", "engine/assets/shaders/post/blit.frag"}} {
}

void BlitFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates descriptors.
    descriptorSet.push("samplerColor", Graphics::Get()->getAttachment("swapchain"));

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
