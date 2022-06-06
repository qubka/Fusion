#include "scene.hpp"
#include "components.hpp"

#include "fusion/renderer/systems/model_renderer.hpp"
#include "fusion/renderer/systems/grid_renderer.hpp"
#include "fusion/renderer/editor_camera.hpp"
#include "fusion/input/input.hpp"

using namespace fe;

Scene::Scene(std::string name) : name{std::move(name)} {
    world.on_construct<TransformComponent>().connect<&entt::registry::emplace<DirtyTransformComponent>>();
    world.on_update<TransformComponent>().connect<&entt::registry::emplace_or_replace<DirtyTransformComponent>>();

    world.on_construct<ModelComponent>().connect<&entt::registry::emplace<DirtyModelComponent>>();
    world.on_update<ModelComponent>().connect<&entt::registry::emplace_or_replace<DirtyModelComponent>>();
}

Scene::~Scene() {

}

void Scene::onViewportResize(const glm::vec2& size) {

}

void Scene::onUpdateRuntime(float dt) {

}

void Scene::onUpdateEditor(float dt) {

}

void Scene::onRenderRuntime() {

}

void Scene::onRenderEditor(const EditorCamera& camera) {

    auto transformGroup = world.group<DirtyTransformComponent, TransformComponent>();
    transformGroup.sort([&](const entt::entity lhs, const entt::entity rhs) {
        auto clhs = world.try_get<RelationshipComponent>(lhs);
        if (clhs == nullptr)
            return false;
        auto crhs = world.try_get<RelationshipComponent>(rhs);
        if (crhs == nullptr)
            return false;
        return !(clhs->parent != entt::null && clhs->children < crhs->children);
    });
    transformGroup.each([&](const auto entity, auto& transform) {
        transform.localToWorldMatrix = world.transform(entity);
        transform.localToWorldMatrix[1] *= -1; // invert Y-Scale for vulkan flipped offscreen
        transform.worldToLocalMatrix = glm::inverse(transform.localToWorldMatrix);
    });

    world.clear<DirtyTransformComponent>();

    auto& modelRenderer = ModelRenderer::Instance();

    auto modelView = world.view<DirtyModelComponent, ModelComponent>();
    modelView.each([&](const auto entity, ModelComponent& model) {
        if (model.path.empty() || !std::filesystem::exists(model.path))
            model.model.reset();
        else
            model.model = modelRenderer.loadModel(model.path);
    });

    world.clear<DirtyModelComponent>();

    modelRenderer.begin();

    world.view<const TransformComponent, const ModelComponent>().each([&](const auto& transform, const auto& model) {
        if (model.model)
            modelRenderer.draw(model.model, transform.localToWorldMatrix);
    });

    modelRenderer.end();

    auto& gridRenderer = GridRenderer::Instance();
    gridRenderer.begin();
    gridRenderer.draw();
    gridRenderer.end();
}