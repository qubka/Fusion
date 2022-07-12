#include "scene_manager.hpp"

using namespace fe;

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
}

void SceneManager::onUpdate() {
    if (!scene)
        return;

    if (!scene->started) {
        scene->onStart();
        scene->started = true;
    }

    scene->onUpdate();
}

