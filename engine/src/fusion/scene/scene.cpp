#include "scene.hpp"
#include "components.hpp"

#include "fusion/renderer/systems/model_renderer.hpp"

using namespace fe;

Scene::Scene() {

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

void Scene::onRenderEditor() {
    auto& renderer = ModelRenderer::Instance();

    renderer.begin();

    auto group = registry.group<TransformComponent>(entt::get<ModelComponent>);
    for (auto entity : group) {
        auto [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
        renderer.draw(transform);
    }

    renderer.end();
}