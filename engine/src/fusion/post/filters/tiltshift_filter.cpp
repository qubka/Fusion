#include "tiltshift_filter.hpp"

using namespace fe;

TiltshiftFilter::TiltshiftFilter(Pipeline::Stage pipelineStage, float blurAmount, float centre, float stepSize, float steps)
        : PostFilter{pipelineStage, {"engine/assets/shaders/post/default.vert", "engine/assets/shaders/post/tiltshift.frag"}}
        , blurAmount{blurAmount}
        , centre{centre}
        , stepSize{stepSize}
        , steps{steps} {
}

void TiltshiftFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("blurAmount", blurAmount);
    pushScene.push("centre", centre);
    pushScene.push("stepSize", stepSize);
    pushScene.push("steps", steps);

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
