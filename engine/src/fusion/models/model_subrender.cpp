#include "model_subrender.hpp"
#include "model.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/scene.hpp"

using namespace fe;

static const uint32_t MAX_LIGHTS = 32; // TODO: Make configurable.

ModelSubrender::ModelSubrender(const Pipeline::Stage& pipelineStage)
        : Subrender{pipelineStage}                                                                          // TODO: Should be same as in Model class
        , pipeline{pipelineStage, {"EngineShaders/simple/simple.vert", "EngineShaders/simple/simple.frag"}, {{{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::Color}}}}
        , descriptorSet{pipeline} {
}

void ModelSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    /*auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Updates uniforms.
    std::vector<PointLight> pointsLights(MAX_LIGHTS);
    uint32_t lightCount = 0;

    auto lightView = scene->getRegistry().view<TransformComponent, LightComponent>();

    for (const auto& [entity, transform, light] : lightView.each()) {
        PointLight pointsLight = {};
        pointsLight.color = light.color;
        pointsLight.position = transform.getWorldPosition();
        pointsLight.radius = light.radius;
        pointsLights[lightCount] = pointsLight;
        lightCount++;

        if (lightCount >= MAX_LIGHTS)
            break;
    }

    // Updates storage buffers.
    storageLights.push(pointsLights.data(), sizeof(PointLight) * MAX_LIGHTS);
    descriptorSet.push("BufferLights", storageLights);

    // Update uniforms
    uniformObject.push("projection", camera->getProjectionMatrix());
    uniformObject.push("view", camera->getViewMatrix());
    uniformObject.push("cameraPos", camera->getEyePoint());
    uniformObject.push("lightsCount", lightCount);
    uniformObject.push("ambientLightColor", glm::vec4{1.0f, 1.0f, 1.0f, 0.02f});
    descriptorSet.push("UniformObject", uniformObject);
    //descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    //pushObject.bindPush(commandBuffer, pipeline);

    auto modelView = scene->getRegistry().view<TransformComponent, MeshComponent>();

    for (const auto& [entity, transform, model] : modelView.each()) {
        pushObject.push("model", transform.getWorldMatrix());
        pushObject.push("normal", transform.getNormalMatrix());
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        model.model->cmdRender(commandBuffer);
    }*/
}