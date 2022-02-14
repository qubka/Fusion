#include "Scene.hpp"
#include "Components.hpp"

#include "Fusion/Systems/MeshRenderer.hpp"

using namespace Fusion;

Scene::Scene() {

}

Scene::~Scene() {

}

void Scene::onUpdateRuntime() {

}

void Scene::onUpdateEditor() {

}

void Scene::onRenderRuntime(MeshRenderer& meshRenderer) {

}

void Scene::onRenderEditor(MeshRenderer& meshRenderer) {
    meshRenderer.beginScene();

    auto group = registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
    for (auto entity : group) {
        auto [transform, mesh] = group.get<TransformComponent, MeshRendererComponent>(entity);
        meshRenderer.drawMesh(transform, mesh.mesh);
    }

    meshRenderer.endScene();
}
