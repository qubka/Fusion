#include "dof_filter.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/scene/scene_manager.hpp"

using namespace fe;

DofFilter::DofFilter(Pipeline::Stage pipelineStage, BlurPipeline* pipelineBlur, float focusPoint, float nearField, float nearTransition, float farField, float farTransition)
        : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/dof.frag"}}
        , pipelineBlur{pipelineBlur}
        , focusPoint{focusPoint}
        , nearField{nearField}
        , nearTransition{nearTransition}
        , farField{farField}
        , farTransition{farTransition} {
}

void DofFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    auto camera = SceneManager::Get()->getScene()->getCamera();
    pushScene.push("nearPlane", camera->getNearClip());
    pushScene.push("farPlane", camera->getFarClip());
    pushScene.push("focusPoint", focusPoint);
    pushScene.push("nearField", nearField);
    pushScene.push("nearTransition", nearTransition);
    pushScene.push("farField", farField);
    pushScene.push("farTransition", farTransition);

    // Updates descriptors.
    descriptorSet.push("PushScene", pushScene);
    descriptorSet.push("samplerDepth", Graphics::Get()->getAttachment("depth"));
    descriptorSet.push("samplerBlured", pipelineBlur ? pipelineBlur->getOutput() : nullptr);
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushScene.bindPush(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}