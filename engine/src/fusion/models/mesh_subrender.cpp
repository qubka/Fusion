#include "mesh_subrender.hpp"
#include "mesh.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/scene.hpp"

using namespace fe;

static const uint32_t MAX_LIGHTS = 32; // TODO: Make configurable.

MeshSubrender::MeshSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage, {"engine/assets/shaders/simple/simple.vert", "engine/assets/shaders/simple/simple.frag"}, {{{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::UV}}}, {},
                   PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::ReadWrite, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE}
        , descriptorSet{pipeline} {
    unknownDiffuse = std::make_unique<Texture2d>("engine/assets/textures/container2.png");
    unknownSpecular = std::make_unique<Texture2d>("engine/assets/textures/container_specular.png");
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
        Light pointsLight = {};
        pointsLight.position = transform.getWorldPosition();
        if (light.type != LightComponent::LightType::Point) {
            pointsLight.direction = transform.getWorldUpDirection();
        }
        if (light.type == LightComponent::LightType::Spot) {
            pointsLight.cutOff = light.cutOff;
            pointsLight.outerCutOff = light.outerCutOff;
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
    uniformObject.push("ambientLightColor", glm::vec4{1.0f, 1.0f, 1.0f, 0.02f});
    descriptorSet.push("UniformObject", uniformObject);
    descriptorSet.push("samplerDiffuse", unknownDiffuse.get());
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
        pushObject.push("normal", transform.getNormalMatrix());
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        mesh.runtime->cmdRender(commandBuffer);
    }
}

// PBR
/*void MeshSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Update uniforms
    uniformObject.push("baseDiffuse", glm::vec4{0.5f, 0.3f, 0.5f, 1.0f});
    uniformObject.push("metallic", 0.5f);
    uniformObject.push("roughness", 0.5f);
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
        pushObject.push("model", transform.getWorldMatrix());
        pushObject.push("normal", transform.getNormalMatrix());
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        mesh.mesh->cmdRender(commandBuffer);
    }
}*/

// SIMPLE
/*void MeshSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Updates uniforms.
    //std::vector<PointLight> pointsLights(MAX_LIGHTS);
    //uint32_t lightCount = 0;

    uniformMat.push("ambient", glm::vec3{1.0f, 0.5f, 0.31f});
    uniformMat.push("diffuse", glm::vec3{1.0f, 0.5f, 0.31f});
    uniformMat.push("specular", glm::vec3{0.5f});
    uniformMat.push("shinines", 32.0f);
    descriptorSet.push("Material", uniformMat);

    float time = sinf(Time::CurrentTime().asSeconds());
    glm::vec3 lightColor { 2.0f, 0.7f, 1.3f };
    uniformLight.push("position", glm::vec3{0.0f});
    uniformLight.push("ambient", lightColor * 0.5f);
    uniformLight.push("diffuse", lightColor * 0.2f);
    uniformLight.push("specular", glm::vec3{1.0f});
    descriptorSet.push("Light", uniformLight);

    // Updates storage buffers.
    //storageLights.push(pointsLights.data(), sizeof(PointLight) * MAX_LIGHTS);
    //descriptorSet.push("BufferLights", storageLights);

    // Update uniforms
    uniformObject.push("projection", camera->getProjectionMatrix());
    uniformObject.push("view", camera->getViewMatrix());
    uniformObject.push("cameraPos", camera->getEyePoint());
    descriptorSet.push("UniformObject", uniformObject);
    //descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    //pushObject.bindPush(commandBuffer, pipeline);

    auto meshView = scene->getRegistry().view<TransformComponent, MeshComponent>();

    for (const auto& [entity, transform, mesh] : meshView.each()) {
        pushObject.push("model", transform.getWorldMatrix());
        pushObject.push("normal", transform.getNormalMatrix());
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        mesh.mesh->cmdRender(commandBuffer);
    }
}*/

// SIMPLE 2
/*void MeshSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
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

    auto meshView = scene->getRegistry().view<TransformComponent, MeshComponent>();

    for (const auto& [entity, transform, mesh] : meshView.each()) {
        pushObject.push("model", transform.getWorldMatrix());
        pushObject.push("normal", transform.getNormalMatrix());
        descriptorSet.push("PushObject", pushObject);
        pushObject.bindPush(commandBuffer, pipeline);

        mesh.mesh->cmdRender(commandBuffer);
    }
} */