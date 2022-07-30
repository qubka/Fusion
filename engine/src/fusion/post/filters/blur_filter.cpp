#include "blur_filter.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

BlurFilter::BlurFilter(Pipeline::Stage pipelineStage, const glm::vec2& direction, Type type)
    : PostFilter{pipelineStage, {"engine/assets/shaders/post/default.vert", "engine/assets/shaders/post/blur.frag"}, getDefines(type)}
    , type{type}
    , direction{direction} {
}

void BlurFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("direction", direction);

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

std::vector<Shader::Define> BlurFilter::getDefines(const Type& type) {
    std::vector<Shader::Define> defines;
    defines.emplace_back(Shader::Define("BLUR_TYPE", std::to_string(me::enum_integer(type))));
    return defines;
}
