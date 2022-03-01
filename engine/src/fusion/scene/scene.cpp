#include "scene.hpp"
#include "components.hpp"

//#include "fusion/systems/meshrenderer.hpp"

using namespace fe;

Scene::Scene() {

}

Scene::~Scene() {

}

void Scene::onUpdateRuntime(float ts) {

}

void Scene::onUpdateEditor(float ts) {

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
