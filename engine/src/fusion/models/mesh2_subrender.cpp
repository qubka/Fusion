#include "mesh2_subrender.h"
#include "mesh.h"

#include "fusion/scene/scene_manager.h"
#include "fusion/scene/components.h"
#include "fusion/scene/scene.h"
#include "fusion/graphics/graphics.h"

using namespace fe;

static const uint32_t MAX_LIGHTS = 32; // TODO: Make configurable.

Mesh2Subrender::Mesh2Subrender(Pipeline::Stage pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage,
               {FUSION_ASSET_PATH "shaders/simple/simple2.vert", FUSION_ASSET_PATH "shaders/simple/simple2.frag"},
               {{{
                     Vertex::Component::Position,
                     Vertex::Component::Normal,
                     Vertex::Component::Tangent,
                     Vertex::Component::Bitangent,
                     Vertex::Component::UV
                 }}},
               {{"blinnPhongEnabled", true}}}
    , descriptorSet{pipeline} {
}

void Mesh2Subrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    auto& registry = scene->getRegistry();

    // Updates uniforms.
    std::vector<Light> lights(MAX_LIGHTS);
    uint32_t lightCount = 0;

    {
        auto view = registry.view<TransformComponent, LightComponent>();

        for (const auto& [entity, transform, light]: view.each()) {
            Light baseLight = {};
            baseLight.position = transform.getWorldPosition();
            if (light.type != LightComponent::LightType::Point) {
                baseLight.direction = transform.getWorldForwardDirection();
            }
            if (light.type == LightComponent::LightType::Spot) {
                baseLight.cutOff = glm::cos(glm::radians(light.cutOff));
                baseLight.outerCutOff = glm::cos(glm::radians(light.outerCutOff));
            }
            baseLight.ambient = light.ambient;
            baseLight.diffuse = light.diffuse;
            baseLight.specular = light.specular;
            baseLight.constant = light.constant;
            baseLight.linear = light.linear;
            baseLight.quadratic = light.quadratic;
            lights[lightCount] = baseLight;
            lightCount++;

            if (lightCount >= MAX_LIGHTS)
                break;
        }
    }

    // Updates storage buffers.
    storageLights.push(lights.data(), sizeof(Light) * MAX_LIGHTS);
    descriptorSet.push("BufferLights", storageLights);

    // Update uniforms
    uniformObject.push("projection", camera->getProjectionMatrix());
    uniformObject.push("view", camera->getViewMatrix());
    uniformObject.push("cameraPos", camera->getEyePoint());
    uniformObject.push("lightsCount", lightCount);
    descriptorSet.push("UniformObject", uniformObject);
    //descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    //descriptorSet.bindDescriptor(commandBuffer, pipeline);

    auto group = registry.group<MeshComponent>(entt::get<TransformComponent, MaterialComponent>);

    group.sort([&registry](const entt::entity a, const entt::entity b) {
        return registry.get<MeshComponent>(a).get() < registry.get<MeshComponent>(b).get();
    });

    for (const auto& [entity, mesh, transform, material] : group.each()) {
        auto filter = mesh.get();
        if (!filter)
            continue;

        pushObject.push("model", transform.getWorldMatrix());

        glm::mat4 normal{ transform.getNormalMatrix() };

        normal[3] = glm::vec4{material.baseColor, material.shininess};
        pushObject.push("normal", normal);

        descriptorSet.push("PushObject", pushObject);

        pushObject.bindPush(commandBuffer, pipeline);

        descriptorSet.bindDescriptor(commandBuffer, pipeline);

        filter->cmdRender(commandBuffer);
    }
}
