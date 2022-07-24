#include "wobble_filter.hpp"
#include "fusion/core/time.hpp"

using namespace fe;

WobbleFilter::WobbleFilter(const Pipeline::Stage& pipelineStage, float wobbleSpeed)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/wobble.frag"}}
        , wobbleSpeed{wobbleSpeed}
        , wobbleAmount{0.0f} {
}

void WobbleFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    wobbleAmount += wobbleSpeed * Time::DeltaTime().asSeconds();

    // Updates uniforms
    pushScene.push("moveIt", wobbleAmount);

    // Updates descriptors
    descriptorSet.push("PushScene", pushScene);
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushScene.bindPush(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}