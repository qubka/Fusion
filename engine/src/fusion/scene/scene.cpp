#include "scene.hpp"
#include "components.hpp"

//#include "fusion/systems/meshrenderer.hpp"

using namespace Fusion;

Scene::Scene() {

}

Scene::~Scene() {

}

void Scene::onUpdateRuntime() {

}

void Scene::onUpdateEditor() {

}

void Scene::onRenderRuntime() {

}

void Scene::onRenderEditor() {
    /*meshRenderer.beginScene();

    auto group = registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
    for (auto entity : group) {
        auto [transform, mesh] = group.get<TransformComponent, MeshRendererComponent>(entity);
        meshRenderer.drawMesh(transform, mesh.mesh);
    }

    meshRenderer.endScene();*/
}
