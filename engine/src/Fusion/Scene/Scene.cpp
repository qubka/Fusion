#include "Scene.hpp"
#include "Components.hpp"

using namespace Fusion;

Scene::Scene() {

}

Scene::~Scene() {

}

void Scene::onUpdateRuntime() {

}

void Scene::onUpdateEditor(EditorCamera& camera) {

}

entt::entity Scene::getPrimaryCameraEntity() {
    entt::entity result;
    return result;
}
