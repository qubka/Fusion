#include "atmosphere_subrender.h"

#include "fusion/graphics/commands/command_buffer.h"
#include "fusion/graphics/cameras/camera.h"
#include "fusion/devices/device_manager.h"
#include "fusion/scene/scene_manager.h"

using namespace fe;

AtmosphereSubrender::AtmosphereSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage, {FUSION_ASSET_PATH "shaders/sky/atmosphere.vert", FUSION_ASSET_PATH "shaders/sky/atmosphere.frag"}}
        , descriptorSet{pipeline} {
}

AtmosphereSubrender::~AtmosphereSubrender() {

}

void AtmosphereSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    auto camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Updates uniform
    pushObject.push("projection", camera->getProjectionMatrix());
    pushObject.push("view", camera->getViewMatrix());

    // Updates descriptors
    descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}
