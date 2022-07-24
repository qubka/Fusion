#include "lensflare_filter.hpp"

#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/scene/scene_manager.hpp"

using namespace fe;

LensflareFilter::LensflareFilter(const Pipeline::Stage& pipelineStage)
    : PostFilter{pipelineStage, {"EngineShaders/post/default.vert", "EngineShaders/post/lensflare.frag"}} {
}

void LensflareFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    // Updates uniforms.
    pushScene.push("sunPosition", sunPosition);
    pushScene.push("displaySize", pipeline.getRenderArea().extent);
    pushScene.push("worldHeight", sunHeight);

    // Updates descriptors.
    descriptorSet.push("PushScene", pushScene);
    descriptorSet.push("samplerMaterial", getAttachment("samplerMaterial", "material"));
    pushConditional("writeColor", "samplerColor", "resolved", "diffuse");

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushScene.bindPush(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void LensflareFilter::setSunPosition(const glm::vec3& position, const glm::vec2& size) {
    auto camera = SceneManager::Get()->getScene()->getCamera();
    glm::vec4 viewport{ 0, 0, size.x, size.y };
    sunPosition = glm::project(position, camera->getViewMatrix(), camera->getProjectionMatrix(), viewport);
}