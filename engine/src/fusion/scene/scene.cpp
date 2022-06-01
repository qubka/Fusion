#include "scene.hpp"
#include "components.hpp"

#include "fusion/renderer/systems/model_renderer.hpp"
#include "fusion/renderer/systems/grid_renderer.hpp"
#include "fusion/renderer/editor_camera.hpp"

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

void Scene::onRenderEditor(const EditorCamera& camera) {
    auto& modelRenderer = ModelRenderer::Instance();

    modelRenderer.begin();

    auto group = manager.group<TransformComponent>(entt::get<ModelComponent>);
    for (auto entity : group) {
        auto [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
        modelRenderer.draw(transform.invTransform());
    }

    modelRenderer.end();

    auto& gridRenderer = GridRenderer::Instance();

    gridRenderer.begin();

    gridRenderer.draw();

    gridRenderer.end();
}