#include "scene.hpp"
#include "components.hpp"

//#include "fusion/systems/meshrenderer.hpp"

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

void Scene::onRenderRuntime(Renderer& renderer) {

}

void Scene::onRenderEditor(Renderer& renderer) {
    /*meshRenderer.beginScene();

    auto group = registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
    for (auto entity : group) {
        auto [transform, mesh] = group.get<TransformComponent, MeshRendererComponent>(entity);
        meshRenderer.drawMesh(transform, mesh.mesh);
    }

    meshRenderer.endScene();*/
}