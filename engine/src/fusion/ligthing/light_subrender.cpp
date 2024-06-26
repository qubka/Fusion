#include "light_subrender.h"

#include "fusion/graphics/commands/command_buffer.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/scene/components.h"

using namespace fe;

LightSubrender::LightSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage,
                   {FUSION_ASSET_PATH "shaders/simple/light.vert", FUSION_ASSET_PATH "shaders/simple/light.frag"},
                   {}, {},
                   PipelineGraphics::Mode::Polygon,
                   PipelineGraphics::Depth::ReadWrite,
                   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                   VK_POLYGON_MODE_FILL,
                   VK_CULL_MODE_FRONT_BIT,
                   VK_FRONT_FACE_CLOCKWISE}
        , descriptorSet{pipeline} {
}

void LightSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    auto camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    auto& registry = scene->getRegistry();

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

    auto lightView = registry.view<TransformComponent, LightComponent>();

    for (const auto& [entity, transform, light] : lightView.each()) {
        if (light.type == LightComponent::LightType::Directional)
            continue;

        pushObject.push("color", light.color);
        pushObject.push("position", glm::vec4{ transform.getWorldPosition(), light.radius });
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
    }
}
