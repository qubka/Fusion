#include "mesh_subrender.h"
#include "mesh.h"

#include "fusion/scene/scene_manager.h"
#include "fusion/scene/components.h"
#include "fusion/scene/scene.h"
#include "fusion/graphics/graphics.h"

using namespace fe;

static const uint32_t MAX_LIGHTS = 32; // TODO: Make configurable.

MeshSubrender::MeshSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage,
                   {FUSION_ASSET_PATH "shaders/simple/simple.vert", FUSION_ASSET_PATH "shaders/simple/simple.frag"},
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

void MeshSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
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

    bindlessDescriptors.clear();

    auto view = registry.view<MaterialComponent>();

    float i = 0.0f;
    for (const auto& [entity, material]: view.each()) {
        if (material.diffuse && *material.diffuse) if (bindlessDescriptors.emplace(material.diffuse.get(), i).second) i++;
        if (material.specular && *material.specular) if (bindlessDescriptors.emplace(material.specular.get(), i).second) i++;
        if (material.normal && *material.normal) if (bindlessDescriptors.emplace(material.normal.get(), i).second) i++;
    }

    descriptorSet.push("textures", bindlessDescriptors.keys());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);

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
        normal[0].w = material.diffuse && *material.diffuse ? bindlessDescriptors[material.diffuse.get()] : -1.0f;
        normal[1].w = material.specular && *material.specular ? bindlessDescriptors[material.specular.get()] : -1.0f;
        normal[2].w = material.normal && *material.normal ? bindlessDescriptors[material.normal.get()] : -1.0f;
        normal[3] = glm::vec4{material.baseColor, material.shininess};
        pushObject.push("normal", normal);

        descriptorSet.push("PushObject", pushObject);

        pushObject.bindPush(commandBuffer, pipeline);
        filter->cmdRender(commandBuffer);
    }
}

// PBR
/*
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Update uniforms
    uniformObject.push("baseDiffuse", glm::vec4{0.5f, 0.3f, 0.5f, 1.0f});
    uniformObject.push("metallic", 0.0f);
    uniformObject.push("roughness", 1.0f);
    uniformObject.push("ignoreFog", 0.0f);
    uniformObject.push("ignoreLighting", 0.0f);
    descriptorSet.push("UniformObject", uniformObject);

    uniformScene.push("projection", camera->getProjectionMatrix());
    uniformScene.push("view", camera->getViewMatrix());
    uniformScene.push("cameraPos", camera->getEyePoint());
    descriptorSet.push("UniformScene", uniformScene);
    //descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    //pushObject.bindPush(commandBuffer, pipeline);

    auto meshView = scene->getRegistry().view<TransformComponent, MeshComponent>();

    for (const auto& [entity, transform, mesh] : meshView.each()) {
        if (!mesh.runtime)
            continue;

        pushObject.push("model", transform.getWorldMatrix());
        pushObject.push("normal", glm::mat4{transform.getNormalMatrix()});
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        mesh.runtime->cmdRender(commandBuffer);
    }
 */

// Simple 3
/*
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Updates uniforms.
    std::vector<Light> lights(MAX_LIGHTS);
    uint32_t lightCount = 0;

    auto lightView = scene->getRegistry().view<TransformComponent, LightComponent>();

    for (const auto& [entity, transform, light] : lightView.each()) {
        Light pointsLight = {};
        pointsLight.position = transform.getWorldPosition();
        if (light.type != LightComponent::LightType::Point) {
            pointsLight.direction = transform.getWorldDownDirection();
        }
        if (light.type == LightComponent::LightType::Spot) {
            pointsLight.cutOff = glm::cos(glm::radians(light.cutOff));
            pointsLight.outerCutOff = glm::cos(glm::radians(light.outerCutOff));
        }
        pointsLight.ambient = light.ambient;
        pointsLight.diffuse = light.diffuse;
        pointsLight.specular = light.specular;
        pointsLight.constant = light.constant;
        pointsLight.linear = light.linear;
        pointsLight.quadratic = light.quadratic;
        lights[lightCount] = pointsLight;
        lightCount++;

        if (lightCount >= MAX_LIGHTS)
            break;
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
    descriptorSet.push("samplerDiffuse", unknownDiffuse.get());
    descriptorSet.push("samplerNormal", unknownNormal.get());
    descriptorSet.push("samplerSpecular", unknownSpecular.get());
    //descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    //pushObject.bindPush(commandBuffer, pipeline);

    auto meshView = scene->getRegistry().view<TransformComponent, MeshComponent>();

    for (const auto& [entity, transform, mesh] : meshView.each()) {
        if (!mesh.runtime)
            continue;

        pushObject.push("model", transform.getWorldMatrix());
        pushObject.push("normal", glm::mat4{transform.getNormalMatrix()});
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        mesh.runtime->cmdRender(commandBuffer);
    }
 */