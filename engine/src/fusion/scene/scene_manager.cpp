#include "scene_manager.h"

#include "fusion/devices/device_manager.h"

using namespace fe;

SceneManager::~SceneManager() {
    //if (started) DeviceManager::Get()->getWindow(0)->OnResize().connect<&SceneManager::onWindowResize>(this);
}

void SceneManager::onStart() {
    //DeviceManager::Get()->getWindow(0)->OnResize().connect<&SceneManager::onWindowResize>(this);
    // TODO: Delete hook ?
}

void SceneManager::onUpdate() {
    if (!scene)
        return;

    if (!scene->started) {
        scene->onStart();
        scene->started = true;
        if (scene->runtime)
            scene->onPlay();
    }

    scene->onUpdate();
}

void SceneManager::onWindowResize(const glm::uvec2& size) {
    if (scene) {

    }
}

bool SceneManager::cacheScene() {
    if (!scene)
        return false;

    sceneCached = std::move(scene);
    scene = std::make_unique<Scene>(*sceneCached);
    scene->runtime = true;
    return true;
}

bool SceneManager::loadCachedScene() {
    if (!sceneCached)
        return false;

    if (scene->runtime)
        scene->onStop();
    scene = std::move(sceneCached);
    return true;
}
