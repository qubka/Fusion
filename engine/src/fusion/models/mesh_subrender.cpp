#include "mesh_subrender.hpp"
#include "mesh.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/scene.hpp"

using namespace fe;

static const uint32_t MAX_LIGHTS = 32; // TODO: Make configurable.

MeshSubrender::MeshSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage, {"engine/assets/shaders/simple/simple.vert", "engine/assets/shaders/simple/simple.frag"}, {{{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::Tangent, Vertex::Component::Bitangent, Vertex::Component::UV, Vertex::Component::UV}}}}
        , descriptorSet{pipeline} {
    unknownDiffuse = std::make_unique<Texture2d>("engine/assets/textures/Diffuse.png");
    unknownSpecular = std::make_unique<Texture2d>("engine/assets/textures/Diffuse_Spec.png");
    unknownNormal = std::make_unique<Texture2d>("engine/assets/textures/Diffuse_Normal.png");
}

void MeshSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
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
        Light baseLight = {};
        baseLight.position = transform.getWorldPosition();
        if (light.type != LightComponent::LightType::Point) {
            baseLight.direction = transform.getWorldDownDirection();
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