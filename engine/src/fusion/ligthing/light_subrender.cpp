#include "light_subrender.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/scene.hpp"

using namespace fe;

LightSubrender::LightSubrender(const Pipeline::Stage& pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage, {"EngineShaders/simple/light.vert", "EngineShaders/simple/light.frag"}}
        , descriptorSet{pipeline} {
}

void LightSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Update uniforms
    uniformObject.push("projection", camera->getProjectionMatrix());
    uniformObject.push("view", camera->getViewMatrix());
    descriptorSet.push("UniformObject", uniformObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    //pushObject.bindPush(commandBuffer, pipeline);

    auto lightView = scene->getRegistry().view<TransformComponent, LightComponent>();

    for (const auto& [entity, transform, light] : lightView.each()) {
        pushObject.push("color", light.color);
        pushObject.push("position", transform.getWorldPosition());
        pushObject.push("radius", light.radius);
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
    }
}
