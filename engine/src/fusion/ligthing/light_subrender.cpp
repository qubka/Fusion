#include "light_subrender.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"

using namespace fe;

LightSubrender::LightSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage, {"engine/assets/shaders/simple/light.vert", "engine/assets/shaders/simple/light.frag"}, {}, {},
                   PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE}
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
        if (light.type == LightComponent::LightType::Directional)
            continue;

        pushObject.push("color", light.color);
        pushObject.push("position", glm::vec4{ transform.getWorldPosition(), light.radius });
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
    }
}
